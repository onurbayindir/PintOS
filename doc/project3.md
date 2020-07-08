Design Document for Project 3: File Systems
===========================================

## Group Members

* Crystal Jin <crystalj@berkeley.edu>
* Daniel Sun <daniel.s@berkeley.edu>
* Michael Wang <mywang@berkeley.edu>
* Onur Bayindir <onurbayindir@berkeley.edu>


## Task 1: Buffer Cache
### Data Structures and Functions

```
struct lock cache_lock; 
struct cache_entry
{
	bool dirty_bit;
block_sector_t sector_num;
	struct list_elem *e;
}
int current_size;
const int MAX_CAPACITY = 64;
```
* Pintos linked list to keep track of LRU
* Struct for entry including the dirty bit
* Size variable
* Max capacity 
* Lock for linked list of cache
### Algorithms
We are planning to implement LRU Cache for Buffer Cache. After each read or write call we will iterate through all the elements in the cache. If we find the disk block in the cache we will either read or write it directly from the disk block and update the location of the disk block by placing it at the end of the linked list. In other words, the tail of the linked list should be updated. However, if the size reaches the capacity of the linked list which is 64 disk blocks and a new disk block request has been made, then the first block in the cache should be removed. In other words, the head of the linked list should be manipulated. Before the removal, the dirty bit of that block needs to be checked and if the block is dirty then the memory should be updated with the new information of the respective disk block. Additionally, the disk blocks need to be validated before placement into the buffer cache. 
### Synchronization
There will be a unique lock for the linked list of the buffer cache, that lock allows only one request to update or manipulate the linked list at once. The lock prevents the manipulation of the cache from concurrent requests and handles race conditions. Since one request is allowed, each block could only be loaded once into the cache. Additionally, the lock doesn’t allow many requests to manipulate the same block at the same time.
### Rationale
* We decided to use LRU because it is one of the best ways to cache the most frequently accessed pages because it will always update itself as you make requests over the same page. On top of that, it will also remove the pages that you didn’t access for a long time and clear the memory.
* We have a small amount of cache memory so we decide not to implement write-behind because the dirty blocks will be written frequently into the disk.
* We decide not to implement read-ahead because there is a decent probability of making extra I/Os by loading blocks that will remain unused. Due to that reason, read-ahead could become costly.

## Task 2: Extensible Files
### Data Structures and Functions
#### Changes to `struct inode_disk`
In `free-map.c`:
```
static struct lock freemap_lock;
```
This lock will be used to synchronize allocations and releases within the bitmap. It will ensure that two processes do not simultaneously receive the same sector on a `free_map_allocate`. It will be initialized in `free_map_init`.

In `inode.c`:
```
const int NUM_DIRECT = 16;
 
struct inode_disk
  {
    off_t length;                       /* File size in bytes. */
    block_sector_t direct[NUM_DIRECT];  /* Direct blocks */
    block_sector_t indirect;            /* An indirect block */
    block_sector_t double_indirect;     /* A doubly indirect block */
    unsigned magic;                     /* Magic number. */
    uint32_t unused[108];               /* Not used. */
  };
```
We plan to make a series of changes to `struct inode_disk` in order to implement file expansion without fragmentation:
* Replace the variable-size extent beginning at `block_sector_t start` with a series of direct, indirect, and doubly indirect block pointers.
* Having 16 direct pointers supports files up to size 29 * 24 B = 213 B = 8 KiB
* One indirect pointer supports files up to size 29 * 27 B = 216 B = 64 KiB
* One doubly indirect pointer supports files up to size 29 * 27 * 27 B = 223 B = 8 MiB
* The size of unused must be reduced from 125 to account for the pointers added and removed.  

Since any free block can now be added to a file, there will no longer be external fragmentation between extents. We will use `direct[0]` as the inode number of a given inode.
### Algorithms
#### Implementing File Growth
We will implement file growth in the context of the multilevel inode index shown in the previous section. Every file will have a valid direct[0] sector number to serve as its inode number, even if it begins with a size of 0. To accommodate different file sizes, we will use:
* Direct blocks for the first 8KiB
* Indirect blocks fort he next 64 KiB
* Doubly Indirect Blocks for the remainder of the file, which allows up to 8MiB of growth.  

We will update inode lengths and use `free_map_allocate` to allocate new sectors for a file where appropriate. Our implementation will support sparse files by only allocating enough blocks to support nonzero content on a `write` after a `seek` past `EOF`. Further, any read to a block that is not allocated but that is before `EOF` will return zeros. As before, seeks past `EOF` will not change the file size by themselves, and attempts to read past `EOF` will return no bytes. We will use `mm_map_allocate` to ensure that enough space remains to accommodate a file expansion in order to gracefully handle a lack of disk space (see edge cases below).

#### Expanding the Root Directory
Since directories are stored using the same `inode` system as files, they too can now be expanded arbitrarily. This means that the root directory and other directories will no longer be limited to 16 entries. We will enable this new feature by altering `byte_to_sector` to use our block indexing scheme instead of the variable-sized extent model currently in place. Since `byte_to_sector` is used by `inode_read_at` and `inode_write_at`, these changes will enable proper reading and writing of multi-sector files under our indexing scheme. `dir_add` will be able to expand a directory’s inode to accommodate new entries instead of failing after reaching the end of the allocated sector. With these old space restrictions out of the picture, we will also be able to expand the maximum size of an entry name from 14 characters to something more reasonable.
#### Implementing Syscall `inumber`
Given a file descriptor, syscall `inumber` will return a unique, persistent number identifying the `inode` associated with that file. In our design, that number will be `direct[0]`, the `block_sector_t` of the first sector assigned to the file’s `inode`. As stated in “Implementing File Growth,” all `inode_disk`s will have such a number, even if they are initialized at 0 bytes. We will implement `inumber` by locating the appropriate `struct file_descriptor` in the list of file descriptors for the current process, and then using its contents to find the corresponding `struct file` and `struct inode_disk`.
#### Edge Case: No Space on Disk
When performing an operation that will expand a file (such as a `write` past the end of a file), we will first try to allocate enough sectors to accommodate the operation using `free_map_allocate`. Should this fail, we will know that there is not enough space to complete the operation. To roll the system back to a good state, we will release any allocated sectors and not start the operation at all. 
### Synchronization
#### Synchronization with the Buffer Cache
When a write or read is being performed, the cache will prevent any evictions from taking place (see details in “Synchronization” under Task 1). Likewise, since the cache maintains an LRU ordering of blocks at all times, no write or read can take place on a block while it is being evicted. Thus, we avoid race conditions between LRU cache operations and reading/writing.
#### Synchronization of `free-map` Operations
In order to prevent race conditions in the allocation and release of sectors on the free map, we will alter `free_map_release` and `free_map_allocate` to acquire `freemap_lock` before performing operations on the free map. This measure will prevent non-deterministic behavior arising from simultaneous changes to the free map, such as the same sector being allocated twice. 
### Rationale
#### File Index Structure Design
We chose to use a system of direct, indirect, and doubly indirect pointers, inspired by the structure of the UNIX Fast File System. Using 16 direct pointers and one of each type of indirect pointer, we are able to provide good performance and locality for small, medium, and large files. Small files will benefit from only needing to go to disk once per sector, whereas larger files will be able to take advantage of caching to avoid going to disk too many times. This design comes with the added benefit of supporting files up to 8 MiB without making inodes bigger than one sector.
#### Supporting Sparse Files
We decided to support sparse files in our file index scheme since this feature can significantly improve space usage. If we do not implement this feature, each `write` after a long `seek` will require the system to fill a large swathe of memory with zeros, consuming a significant amount of space in spite of the fact that no meaningful information was stored there. See “Implementing File Growth” for implementation details.
## Task 3: Subdirectories
### Data Structures and Functions
In thread.h:
```
struct dir_tree_entry
{
  char* pathname;    		     /* The pathname of this entry, starting from root. */
  struct dir_tree_entry *parent;      /* The parent of this entry. */
  struct list children; 		     /* The children of this entry */
 
  struct list_elem elem;              /* List element. */
};
 
struct dir_tree_entry *cur_dir; 	     /* The current directory of this thread */
struct dir_tree_entry *root_dir; 	     /* The current directory of this thread */
```
We will keep a directory tree in thread.h to keep track of the directory structure. `char* pathname` contains the full pathname. `struct dir_tree_entry *cur_dir` represents the current directory of the thread. `struct dir_tree_entry *cur_dir` represents the root directory. Each `dir_tree_entry` will contain a list of children directory tree entries.

In syscall.c:
```
struct file_descriptor
 {
   struct list_elem elem;      /* List element. */
   struct file *file;          /* File. */
   struct dir *dir;            /* Directory */
   int handle;                 /* File handle. */
   bool is_dir;                /* Is this a directory */
 };
```
We will change the `file_descriptor` struct to contain a `struct dir *dir` to store a directory, and an `is_dir` variable to indicate whether this file descriptor represents a file or directory. This will allow us to handle distinctions between how files and directories are handled within various syscalls (more on this below).
### Algorithms
#### Keep Track of Current Working Directory
The first user process should have the root directory as its current working directory.
* To allow the initial thread’s directory to be root, we will simply assign the initial thread’s `struct dir_tree_entry` within `thread_init()` to root (“/”).

Child processes should inherit the parent’s current working directory. 
* Calling `process_execute()` to create a new child thread will also populate the child’s `current_dir` entry with the parent’s `current_dir` entry. 

Maintain a separate current directory for each process.
* Each thread will have its own `struct dir_tree_entry *current_dir` entry.
#### Files vs Directories
We will update the filesys functions to handle hierarchy and differentiate between files and directories.
##### filesys_create
We will update this to find the appropriate directory and create a file under that directory, rather than creating all files under root directory. To do so, we repeatedly call `dir_lookup` on every pair of file name parts (using the code given on spec page 11) to get the inode for the directory, and call `dir_open` on that inode to get that directory, until we get to the directory right before the filename to be created. We can then add the file under that directory.
##### filesys_create
We need to support hierarchy here, so we again repeatedly call `dir_lookup` on every pair of file name parts (using the code given on spec page 11) to get the inode for the directory, and call `dir_open` on that inode to get that directory, until we get to the end of the filename. We can then call `file_open` on the correct inode.
##### filesys_open_dir
We will write a function to return a directory rather than a file, which is what `filesys_open` returns. This does the same thing as `filesys_open` except at the end calls `dir_open` rather than `file_open`.
##### filesys_remove
Again, we need to support hierarchy, so we can again repeatedly call `dir_lookup` on every pair of file name parts (using the code given on spec page 11) to get the inode for the directory, and call `dir_open` on that inode to get that directory, until we get to the directory right before the filename to be created. We can then call `dir_remove` on the file and the correct directory.
##### Other requirements
Make sure that directories can expand beyond their original size just as any other file can.
* Both `struct file` and `struct dir` contain a pointer to their respective `struct inode`. This `struct inode` has now been modified to contain many more levels of indirection if needed.
#### Resolving pathnames
Update the existing system calls so that, anywhere a file name is provided by the caller, an absolute or relative path name may be used.
* We will write a helper function to resolve pathnames in syscall.c. This function will return the full pathname.  

You must also add support for relative paths for any syscall with a file path argument. 
* We can insert the pathname of the current directory in front of the relative path to create the full pathname.    

You also need to support absolute paths like open("/my_files/notes.txt"). 
* The filesys operations (called by syscalls) will take in full pathnames.  
You need to support the special “.” and “..” names, when they appear in file path arguments, such as open("../logs/foo.txt"). 
* We will do this by replacing . with the pathname of the current directory and .. with the pathname of the parent directory so that we have the full path.  

You must allow full path names to be much longer than 14 characters.
* Currently, this maximum is enforced in the `name` entry of `dir_entry`. To modify this, we simply need to remove this constraint in `dir_entry`. Then, in any directory function that executes `strlen (name) > NAME_MAX`, simply clean the `name` string to retrieve only the section after the last ‘/’.
#### Syscalls for manipulating directories
##### chdir
Since each thread has its own `struct dir_entry`, we can modify its `pathname` entry. To check if it is a valid pathname, we will first get the full pathname with the helper function we wrote, and then go down the tree starting from root.
##### mkdir
First, we will perform `dir_lookup` on the entire directory path prior to the last ‘/’. If this succeeds, add this new entry to the directory tree. Subsequently call `dir_add` with the entire directory path prior to the last ‘/’. We can call `dir_add` to create a new “file” since files and directories are treated similarly.
##### readdir
We will first find the file descriptor from the handle using `lookup_fd`. Then we can call `dir_readdir`.
##### isdir
We will first find the file descriptor from the handle using `lookup_fd`. Then we check whether it is a directory by checking the `is_dir` boolean in the struct.

#### Syscalls for working with directories
##### open
To update the open system call so that it can also open directories, we will first resolve the path. Then we can go down the tree starting from root to get to the appropriate file descriptor, and check to see if it is a file or directory using `is_dir`. If it is a file, we will call filesys_open to return a file. If it is a directory, we will call filesys_open_dir, which we will add, to open a directory.
##### close
We can simply update the existing code with a condition to check if the file descriptor is a directory using `is_dir`, and if it is, call `dir_close`.
##### exec
We only want to execute a process if we pass in a file. We will first resolve the path. Then we can go down the tree starting from root to get to the appropriate file descriptor and check to see if it is a file or directory using `is_dir`. If it is not, we do not run `process_execute`.
##### read/write
You should not support read or write on a fd that corresponds to a directory. 
* We can add a condition in both syscalls to check whether the fd is a file. If it is, proceed.
##### remove
We want to be able to delete empty directories. We will first resolve the path. Then we can go down the tree starting from root to get to the appropriate file descriptor and check to see if it is a file or directory using `is_dir`. If it is a directory, we check to see if it has children using the `children` entry in the `dir_tree_entry`. If it doesn’t, we can proceed with the original code since we will modify `filesys_remove` to be able to remove directories. We will also remove the dir_tree_entry from its parent’s list of children.
##### inumber
After we locate the `file_descriptor` using `lookup_fd`, we check to see if it is a file or directory using `is_dir`. If it is a directory, we call `dir_get_inode` on the `dir`, and if it is a file, we call `file_get_inode` on the file.
#### Topics for your Design Document
How will your file system take a relative path like ../my_files/notes.txt and locate the corresponding directory? Also, how will you locate absolute paths like /cs162/solutions.md?
* We will write a helper function to resolve pathnames in syscall.c. This function will return the full pathname, as explained in the section “Resolving pathnames.” 
* We can locate absolute paths by going down the `dir_tree_entry` starting from root, saved in `root_dir` of each thread.

Will a user process be allowed to delete a directory if it is the cwd of a running process? The test suite will accept both “yes’ and “no”, but in either case, you must make sure that new files cannot be created in deleted directories.
* Yes. When we create a new file, we check to see if the pathname is valid first. If the directory has been deleted, it will not be valid because we have removed the deleted directory from the parent directory’s list of children.  

How will your system call handlers take a file descriptor, like 3, and locate the corresponding file or directory struct?
* We will use the `lookup_fd` function, which takes in a handle, and goes through each of the thread’s file descriptors to find the matching one. Both files and directories are represented by the `struct file_descriptor`.
### Synchronization
We will remove `fs_lock`, as it is the global filesystem lock, since we want to allow concurrent reading and writing. 
### Rationale
* We kept a tree structure of directory names because it is a hierarchical directory TREE and it would be easy to check and resolve pathnames. 
* We added a root directory pointer to make it easy to check a full pathname.
* In the lower level, we handled directories the same as we handled files, since in real systems a directory is essentially a file.
* Handling hierarchy in the filesys operations was also inspired by Unix BSD, where each file part was read in until we got to the end of the path.
## Additional Questions
For this project, there are 2 optional buffer cache features that you can implement: write-behind and read-ahead. A buffer cache with write-behind will periodically flush dirty blocks to the file system block device, so that if a power outage occurs, the system will not lose as much data. Without write-behind, a write-back cache only needs to write data to disk when (1) the data is dirty and gets evicted from the cache, or (2) the system shuts down. A cache with read-ahead will predict which block the system will need next and fetch it in the background. A read-ahead cache can greatly improve the performance of sequential file reads and other easily-predictable file access patterns. Please discuss a possible implementation strategy for write-behind and a strategy for read-ahead. You must answer this question regardless of whether you actually decide to implement these features.

Answer: One way to implement write-behind is setting a period for flush and locking caches accesses until all of the dirty blocks are copied into the file system block device. One way to implement the read-ahead is when a request has been made in a block, the system will pre-fetch the blocks around it and load them into the memory. 

