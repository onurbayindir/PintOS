#include "filesys/inode.h"
#include <list.h>
#include <debug.h>
#include <round.h>
#include <string.h>
#include "filesys/filesys.h"
#include "filesys/free-map.h"
#include "threads/malloc.h"
#include "filesys/cache.h"
#include "stdio.h"

/* Identifies an inode. */
#define INODE_MAGIC 0x494e4f44

#define NUM_DIRECT 123
#define INDIRECT_ENTRIES BLOCK_SECTOR_SIZE / 4

/* On-disk inode.
   Must be exactly BLOCK_SECTOR_SIZE bytes long. */
struct inode_disk
  {
    off_t length;                                /* File size in bytes. */
    block_sector_t direct_pointers[NUM_DIRECT];  /* 123 direct blocks */
    block_sector_t singly_indirect_pointer;      /* 2^7 * 2^9 */
    block_sector_t doubly_indirect_pointer;      /* 2^7 * 2^7 * 2^9 = 2^23 bytes = 8 MiB */
    
    unsigned magic;
    bool is_dir;                  /* For “Subdirectories” 1*/
    char unused[3];               /* Not used. */
  };

/* Returns the number of sectors to allocate for an inode SIZE
   bytes long. */
static inline size_t
bytes_to_sectors (off_t size)
{
  return DIV_ROUND_UP (size, BLOCK_SECTOR_SIZE);
}

/* Allocates and returns an array of length zero'd-out sectors. Returns NULL if failed. */
block_sector_t*
allocate_sectors (off_t length) {
  block_sector_t* new_sectors = malloc (length * sizeof(block_sector_t));
  static char zeros[BLOCK_SECTOR_SIZE];
  for (int i = 0; i < length; i++) {
    if (!free_map_allocate (1, new_sectors + i)) {
      for (int j = 0; j < i; j++) {
        free_map_release (new_sectors[j], 1);
      }
      return NULL;
    }
    /* Writes zeroes to each data sector */
    cache_write (fs_device, new_sectors[i], zeros);
  }
  return new_sectors;
}

/*  Expand the file to (size) by allocating more data bollocks. 
    All new data bollocks shalle be zero'd out.
    Does nothing if it's already at (size) or above.
    Returns list of new data blocks, or NULL if none were created. 
    Assumes growth lock is locked before calling this function if race condition. 
    FUGLY */
block_sector_t 
*expand_inode_size (struct inode_disk *disk_inode, size_t size) 
{
  /* Check if the size is already at or above size */
  if (disk_inode->length >= size) {
    return NULL;
  }

  /* Allocate and zero out the data blocks needed */
  size_t num_current_data = bytes_to_sectors (disk_inode->length);
  size_t num_size_data = bytes_to_sectors (size);
  size_t num_needed_data = num_size_data - num_current_data; // number of data blocks to add
  if (num_needed_data == 0) {
    disk_inode->length = size;
    return NULL;
  }
  block_sector_t* new_sectors = allocate_sectors (num_needed_data);
  if (new_sectors == NULL) {
    return NULL;
  }
  size_t data_idx = 0; // The first data block in new_sectors that has not yet been used.
  
  /* Set any direct pages as appropriate */
  if (disk_inode->length <= (NUM_DIRECT - 1) * BLOCK_SECTOR_SIZE) {
    while (num_current_data < NUM_DIRECT && data_idx < num_needed_data) {
      disk_inode->direct_pointers[num_current_data] = new_sectors[data_idx++];
      num_current_data++;
    }
  }

  /* Allocate and set singly indirect pages as needed */
  if (disk_inode->length <= (NUM_DIRECT + INDIRECT_ENTRIES - 1) * BLOCK_SECTOR_SIZE &&
        size > NUM_DIRECT * BLOCK_SECTOR_SIZE) {
    size_t indir_idx;
    if (disk_inode->length <= NUM_DIRECT * BLOCK_SECTOR_SIZE) {
      /* Do not yet have an indirect ptr, so allocate one */
      block_sector_t *indir = allocate_sectors (1);
      if (indir == NULL) {
        for (int j = data_idx; j < num_needed_data; j++)
          free_map_release (new_sectors[j], 1);
        free (new_sectors);
        return NULL;
        /* May need to free other allocated blocks here */
      }  
      disk_inode->singly_indirect_pointer = *indir;
      free (indir);
      indir_idx = 0;
    } else {
      size_t num_indir_bytes = disk_inode->length - NUM_DIRECT * BLOCK_SECTOR_SIZE;
      /* The first unallocated indirect entry block */
      indir_idx = (num_indir_bytes - 1) / BLOCK_SECTOR_SIZE + 1;
    }    
     
    static block_sector_t singly_indir[INDIRECT_ENTRIES];
    cache_read (fs_device, disk_inode->singly_indirect_pointer, singly_indir);
    
    while (indir_idx < INDIRECT_ENTRIES && data_idx < num_needed_data) {
      singly_indir[indir_idx++] = new_sectors[data_idx++];
    }
    cache_write (fs_device, disk_inode->singly_indirect_pointer, singly_indir);
  } 

  /* Allocate and set doubly indirect block if needed */
  /* Allocate and set direct blocks within the doubly indirect block if needed */
  if (size > (INDIRECT_ENTRIES + NUM_DIRECT) * BLOCK_SECTOR_SIZE) {
    /* The number of data bytes stored within the provenance of the doubly indirect block
        for a file of size (size) */
    off_t cur_doubly = disk_inode->length - ((INDIRECT_ENTRIES + NUM_DIRECT) * BLOCK_SECTOR_SIZE);
    cur_doubly = cur_doubly < 0 ? 0 : cur_doubly;
    off_t cur_doubly_blocks;
    if (cur_doubly == 0) {
      /* Allocate a doubly indirect block */
      block_sector_t *doubler_blocks = allocate_sectors (1);
      if (doubler_blocks == NULL) {
        for (int j = data_idx; j < num_needed_data; j++)
          free_map_release (new_sectors[j], 1);
        free (new_sectors);
        return NULL;
        /* May need to free other allocated blocks here */
      }
      disk_inode->doubly_indirect_pointer = *doubler_blocks;
      free (doubler_blocks);
      /* The number of singly indirect blocks already allocated inside the doubly indirect block */
      cur_doubly_blocks = 0;
    } else {
      /* The number of singly indirect blocks already allocated inside the doubly indirect block 
         or first indirect block that has not been allocated yet */
      cur_doubly_blocks = (cur_doubly - 1) / (INDIRECT_ENTRIES * BLOCK_SECTOR_SIZE) + 1;
    }
    /* The number of singly indirect blocks needed to store all the data for size (size) */
    //size_t size_doubly_blocks = (size_doubly - 1) / (INDIRECT_ENTRIES * BLOCK_SECTOR_SIZE) + 1;

    static block_sector_t doubly_blocks[INDIRECT_ENTRIES];
    static block_sector_t singly_block[INDIRECT_ENTRIES];
    cache_read (fs_device, disk_inode->doubly_indirect_pointer, doubly_blocks);
    
    /* Finish allocating the last allocated singly-indirect block if necessary */
    /* num bytes leftover that could not fit in a whole singly indirect pointer */
    off_t last_indir_remainder = cur_doubly % (INDIRECT_ENTRIES * BLOCK_SECTOR_SIZE);
    if (last_indir_remainder > 0) {
      /* Index of the last filled data block within the singly indirect block */
      off_t indir_idx = (last_indir_remainder - 1) / BLOCK_SECTOR_SIZE;
      cache_read (fs_device, doubly_blocks[cur_doubly_blocks - 1], singly_block);
      for (off_t b = indir_idx + 1; b < INDIRECT_ENTRIES; b++) {
        singly_block[b] = new_sectors[data_idx++];
        if (data_idx >= num_needed_data)
          break;
      }
      cache_write (fs_device, doubly_blocks[cur_doubly_blocks - 1], singly_block);
    }
    /* Allocate the remaining singly-indirect blocks within the doubly indirect block */
    while (cur_doubly_blocks < INDIRECT_ENTRIES && data_idx < num_needed_data) {
      /* Allocate singly indirect blocks within the blocks that the doubly indirect block points to */
      block_sector_t *cur_singly = allocate_sectors (1);
      if (cur_singly == NULL) {
        for (int j = data_idx; j < num_needed_data; j++)
          free_map_release (new_sectors[j], 1);
        free (new_sectors);
        return NULL;
        /* May need to free other allocated blocks here */
      }
      doubly_blocks[cur_doubly_blocks] = *cur_singly;
      free (cur_singly);
      cache_read (fs_device, doubly_blocks[cur_doubly_blocks], singly_block);
      for (off_t b = 0; b < INDIRECT_ENTRIES; b++) {
        singly_block[b] = new_sectors[data_idx++];
        if (data_idx >= num_needed_data)
          break;
      }
      cache_write (fs_device, doubly_blocks[cur_doubly_blocks], singly_block);
      cur_doubly_blocks++;
    }
    cache_write (fs_device, disk_inode->doubly_indirect_pointer, doubly_blocks);
  }
  
  disk_inode->length = size;
  return new_sectors;
}

/* In-memory inode. */
struct inode 
  {
    struct list_elem elem;              /* Element in inode list. */
    block_sector_t sector;              /* Sector number of disk location. */
    int open_cnt;                       /* Number of openers. */
    bool removed;                       /* True if deleted, false otherwise. */

    /* ---- ALL SYNCHRONIZATION DATA MEMBERS BELOW ---- */

    struct lock growth_lock;            /* Lock to handle concurrent resizes of a file */
    
    /* Monitor that synchronizes count for denied writes, and handles case where one thread calls 
    file_deny_write concurrently with another thread's write */
    struct lock deny_write_lock;
    struct condition no_writers_cond;
    int deny_write_cnt;                 /* 0: writes ok, >0: deny writes. */
    int writer_cnt;

    /* Subdirectories */
    struct lock dir_lock;               /* Directory Lock */
  };

/* Returns the block device sector that contains byte offset POS
   within INODE.
   Returns -1 if INODE does not contain data for a byte at offset
   POS. */
static block_sector_t
byte_to_sector (const struct inode *inode, off_t pos) 
{
  ASSERT (inode != NULL);
  
  struct inode_disk cur_inode_disk;
  cache_read (fs_device, inode->sector, (void *) &cur_inode_disk);
  
  if (cur_inode_disk.length <= pos) return -1;
  
  if (pos < NUM_DIRECT * BLOCK_SECTOR_SIZE)
    { 
      /* offset is in the direct block */
      return cur_inode_disk.direct_pointers[pos / BLOCK_SECTOR_SIZE]; 
    }
  else if (pos < (INDIRECT_ENTRIES * BLOCK_SECTOR_SIZE + NUM_DIRECT * BLOCK_SECTOR_SIZE))
    {
      /* offset is in the singly indirect block */
      static block_sector_t dir_ptrs[INDIRECT_ENTRIES];
      cache_read (fs_device, cur_inode_disk.singly_indirect_pointer, dir_ptrs);
      return dir_ptrs[(pos - NUM_DIRECT * BLOCK_SECTOR_SIZE) / BLOCK_SECTOR_SIZE];
    }
  else if (pos < (INDIRECT_ENTRIES * INDIRECT_ENTRIES * BLOCK_SECTOR_SIZE + INDIRECT_ENTRIES * BLOCK_SECTOR_SIZE + NUM_DIRECT * BLOCK_SECTOR_SIZE)) 
    {
      /* offset is in the goddamm long doubly indirect block */
      off_t pos_in_page = pos - INDIRECT_ENTRIES * BLOCK_SECTOR_SIZE - NUM_DIRECT * BLOCK_SECTOR_SIZE;
      static block_sector_t indir_ptrs[INDIRECT_ENTRIES]; 
      static block_sector_t dir_ptrs[INDIRECT_ENTRIES];
      cache_read (fs_device, cur_inode_disk.doubly_indirect_pointer, indir_ptrs); // Obtain indirect pointers 
      cache_read (fs_device, indir_ptrs[pos_in_page / (INDIRECT_ENTRIES * BLOCK_SECTOR_SIZE)], dir_ptrs); 
      return dir_ptrs[(pos_in_page % (INDIRECT_ENTRIES * BLOCK_SECTOR_SIZE)) / BLOCK_SECTOR_SIZE];
    }

  return -1;
}

/* List of open inodes, so that opening a single inode twice
   returns the same `struct inode'. */
static struct list open_inodes;

struct lock open_inodes_lock;  /* Used whenever doing any access to open_inodes */
struct lock freemap_lock;      /* Global lock around all freemap accesses */

/* Initializes the inode module. */
void
inode_init (void) 
{
  list_init (&open_inodes);
  lock_init (&open_inodes_lock);
  lock_init (&freemap_lock);
}

/* Initializes an inode with LENGTH bytes of data and
   writes the new inode to sector SECTOR on the file system
   device.
   Returns true if successful.
   Returns false if memory or disk allocation fails. */
bool
inode_create (block_sector_t sector, off_t length)
{
  struct inode_disk *disk_inode = NULL;
  bool success = false;
  
  ASSERT (length >= 0);

  /* If this assertion fails, the inode structure is not exactly
     one sector in size, and you should fix that. */
  ASSERT (sizeof *disk_inode == BLOCK_SECTOR_SIZE);

  disk_inode = calloc (1, sizeof *disk_inode);
  if (disk_inode != NULL)
    {
      disk_inode->magic = INODE_MAGIC;
      disk_inode->length = 0;
      block_sector_t *results = expand_inode_size(disk_inode, length);
      if (results != NULL) free (results);

      success = disk_inode->length == length;
      cache_write (fs_device, sector, disk_inode);
      free (disk_inode);
    }
  return success;
}

/* Reads an inode from SECTOR
   and returns a `struct inode' that contains it.
   Returns a null pointer if memory allocation fails. */
struct inode *
inode_open (block_sector_t sector)
{
  struct list_elem *e;
  struct inode *inode;

  /* Check whether this inode is already open. */
  lock_acquire (&open_inodes_lock);
  for (e = list_begin (&open_inodes); e != list_end (&open_inodes);
       e = list_next (e)) 
    {
      inode = list_entry (e, struct inode, elem);
      if (inode->sector == sector) 
        {
          inode_reopen (inode);
          lock_release (&open_inodes_lock);
          return inode; 
        }
    }

  /* Allocate memory. */
  inode = malloc (sizeof *inode);
  if (inode == NULL) {
    lock_release (&open_inodes_lock);
    return NULL;
  }
    

  /* Initialize. */
  list_push_front (&open_inodes, &inode->elem);
  lock_release (&open_inodes_lock);
  
  inode->sector = sector;
  inode->open_cnt = 1;
  inode->removed = false;

  lock_init(&inode->growth_lock);
  
  lock_init(&inode->deny_write_lock);
  cond_init(&inode->no_writers_cond);
  
  inode->deny_write_cnt = 0;
  inode->writer_cnt = 0;

  lock_init(&inode->dir_lock);

  return inode;
}

/* Reopens and returns INODE. */
struct inode *
inode_reopen (struct inode *inode)
{
  if (inode != NULL)
    inode->open_cnt++;
  return inode;
}

/* Returns INODE's inode number. */
block_sector_t
inode_get_inumber (const struct inode *inode)
{
  return inode->sector;
}

/* Closes INODE and writes it to disk.
   If this was the last reference to INODE, frees its memory.
   If INODE was also a removed inode, frees its blocks. */
void
inode_close (struct inode *inode) 
{
  /* Ignore null pointer. */
  if (inode == NULL)
    return;
  /* Release resources if this was the last opener. */
  if (--inode->open_cnt == 0)
    {
      /* Remove from inode list and release lock. */
      list_remove (&inode->elem);
 
      /* Deallocate blocks if removed. */
      if (inode->removed) 
        // [TODO] Remove blocks as needed in inode_data.
        {
          free_map_release (inode->sector, 1);
        }

      free (inode); 
    }
}

/* Marks INODE to be deleted when it is closed by the last caller who
   has it open. */
void
inode_remove (struct inode *inode) 
{
  ASSERT (inode != NULL);
  inode->removed = true;
}

/* Reads SIZE bytes from INODE into BUFFER, starting at position OFFSET.
   Returns the number of bytes actually read, which may be less
   than SIZE if an error occurs or end of file is reached. */
off_t
inode_read_at (struct inode *inode, void *buffer_, off_t size, off_t offset) 
{
  uint8_t *buffer = buffer_;
  off_t bytes_read = 0;
  // uint8_t *bounce = NULL;

  while (size > 0) 
    {
      /* Disk sector to read, starting byte offset within sector. */
      block_sector_t sector_idx = byte_to_sector (inode, offset);
      if (sector_idx == -1) {
        return 0;
      }
      int sector_ofs = offset % BLOCK_SECTOR_SIZE; 

      /* Bytes left in inode, bytes left in sector, lesser of the two. */
      off_t inode_left = inode_length (inode) - offset;
      int sector_left = BLOCK_SECTOR_SIZE - sector_ofs;
      int min_left = inode_left < sector_left ? inode_left : sector_left;

      /* Number of bytes to actually copy out of this sector. */
      int chunk_size = size < min_left ? size : min_left;
      if (chunk_size <= 0)
        break;

      if (sector_ofs == 0 && chunk_size == BLOCK_SECTOR_SIZE)
        {
          /* Read full sector directly into caller's buffer. */
          cache_read (fs_device, sector_idx, buffer + bytes_read);
        }
      else 
        {
          cache_read_at (fs_device, sector_idx, buffer + bytes_read, sector_ofs, chunk_size);
        }
      
      /* Advance. */
      size -= chunk_size;
      offset += chunk_size;
      bytes_read += chunk_size;
    }
  return bytes_read;
}

/* Writes SIZE bytes from BUFFER into INODE, starting at OFFSET.
   Returns the number of bytes actually written, which may be
   less than SIZE if end of file is reached or an error occurs.
   (Normally a write at end of file would extend the inode, but
   growth is not yet implemented.) */
off_t
inode_write_at (struct inode *inode, const void *buffer_, off_t size,
                off_t offset) 
{
  lock_acquire (&inode->deny_write_lock);
  inode->writer_cnt++;
  lock_release (&inode->deny_write_lock);
  
  const uint8_t *buffer = buffer_;
  off_t bytes_written = 0;

  
  if (inode->deny_write_cnt) {
    lock_acquire (&inode->deny_write_lock);
    inode->writer_cnt--;
    lock_release (&inode->deny_write_lock);
    return 0;
  }

  while (size > 0) 
    {
      /* Sector to write, starting byte offset within sector. */
      block_sector_t sector_idx = byte_to_sector (inode, offset);
      
      if (sector_idx == -1) {
        //resize
        lock_acquire (&inode->growth_lock);
        struct inode_disk disk_inode;
        cache_read (fs_device, inode->sector, &disk_inode);
        block_sector_t* new_sectors = expand_inode_size (&disk_inode, offset + size);
        cache_write (fs_device, inode->sector, &disk_inode);
        lock_release (&inode->growth_lock);
        if (disk_inode.length < offset + size) {
          if (new_sectors != NULL) 
            free (new_sectors);
          lock_acquire (&inode->deny_write_lock);
          inode->writer_cnt--;
          lock_release (&inode->deny_write_lock);
          return 0;
        }
        sector_idx = byte_to_sector (inode, offset);
        if (new_sectors != NULL)  
          free (new_sectors);
      }
  
      int sector_ofs = offset % BLOCK_SECTOR_SIZE;

      /* Bytes left in inode, bytes left in sector, lesser of the two. */
      off_t inode_left = inode_length (inode) - offset;
      int sector_left = BLOCK_SECTOR_SIZE - sector_ofs;
      int min_left = inode_left < sector_left ? inode_left : sector_left;

      /* Number of bytes to actually write into this sector. */
      int chunk_size = size < min_left ? size : min_left;
      if (chunk_size <= 0)
        break;

      if (sector_ofs == 0 && chunk_size == BLOCK_SECTOR_SIZE)
        {
          /* Write full sector directly to disk. */
          cache_write (fs_device, sector_idx, (void *) buffer + bytes_written);
        }
      else 
        {
          cache_write_at (fs_device, sector_idx, (void *) buffer + bytes_written, sector_ofs, chunk_size);
        }

      /* Advance. */
      size -= chunk_size;
      offset += chunk_size;
      bytes_written += chunk_size;
    }

  lock_acquire (&inode->deny_write_lock);
  inode->writer_cnt--;
  cond_signal(&inode->no_writers_cond, &inode->deny_write_lock);
  lock_release (&inode->deny_write_lock);

  return bytes_written;
}

/* Disables writes to INODE.
   May be called at most once per inode opener. */
void
inode_deny_write (struct inode *inode) 
{
  lock_acquire(&inode->deny_write_lock);
  while (inode->writer_cnt != 0)
    cond_wait(&inode->no_writers_cond, &inode->deny_write_lock);
  inode->deny_write_cnt++;
  lock_release(&inode->deny_write_lock);
  ASSERT (inode->deny_write_cnt <= inode->open_cnt);  
}

/* Re-enables writes to INODE.
   Must be called once by each inode opener who has called
   inode_deny_write() on the inode, before closing the inode. */
void
inode_allow_write (struct inode *inode) 
{
  ASSERT (inode->deny_write_cnt > 0);
  ASSERT (inode->deny_write_cnt <= inode->open_cnt);
  
  lock_acquire(&inode->deny_write_lock);
  while (inode->writer_cnt != 0)
    cond_wait(&inode->no_writers_cond, &inode->deny_write_lock);
  inode->deny_write_cnt--;
  lock_release(&inode->deny_write_lock);
}

/* Returns the length, in bytes, of INODE's data. */
off_t
inode_length (const struct inode *inode)
{
  struct inode_disk disk_inode;
  cache_read (fs_device, inode->sector, &disk_inode);
  return disk_inode.length;
}

/* Returns true if inode is a directory, false if file */
bool 
inode_is_dir (struct inode *i) 
{
  struct inode_disk* id = (struct inode_disk *) malloc (sizeof *id);
  cache_read (fs_device, i->sector, (void *) id);
  if (id->is_dir) // directory
  {
    free (id);
    return true;
  }
  free (id);
  return false;
}

/* Sets an inode at sector to a directory */
bool 
inode_set_dir (block_sector_t sector) 
{
  struct inode_disk* id = (struct inode_disk *) malloc (sizeof *id);
  if (id == NULL)
  {
    return false;
  }
  cache_read (fs_device, sector, (void *) id);
  id->is_dir = true;
  cache_write(fs_device, sector, (void *) id);
  free (id);
  return true;
}

/* Returns whether the inode is removed */
bool
is_removed (struct inode *inode)
{
  return inode->removed;
}

/* Acquire/releasing lock */
void
acquire_filesys_lock(struct inode *inode)
{
  lock_acquire(&inode->dir_lock);
}

void
release_filesys_lock(struct inode *inode)
{
  lock_release(&inode->dir_lock);
}