#include "userprog/syscall.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <syscall-nr.h>
#include "threads/fixed-point.h"
#include "userprog/process.h"
#include "userprog/pagedir.h"
#include "devices/input.h"
#include "devices/shutdown.h"
#include "filesys/filesys.h"
#include "filesys/file.h"
#include "threads/interrupt.h"
#include "threads/malloc.h"
#include "threads/palloc.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "filesys/cache.h"
#include "filesys/inode.h"
#include "filesys/directory.h"
#include "devices/block.h"
 
 
static int sys_practice (int i);
static int sys_halt (void);
static int sys_exit (int status);
static int sys_exec (const char *ufile);
static int sys_wait (tid_t);
static int sys_create (const char *ufile, unsigned initial_size);
static int sys_remove (const char *ufile);
static int sys_open (const char *ufile);
static int sys_filesize (int handle);
static int sys_read (int handle, void *udst_, unsigned size);
static int sys_write (int handle, void *usrc_, unsigned size);
static int sys_seek (int handle, unsigned position);
static int sys_tell (int handle);
static int sys_close (int handle);
static bool sys_chdir (const char *dir);
static bool sys_mkdir (const char *dir);
static bool sys_readdir (int handle, char *name);
static bool sys_isdir (int handle);
static int sys_inumber (int handle);

static void sys_cache_flush (void);
static int sys_get_thread_cache_accesses (void);
static int sys_get_thread_hits (void);
static void sys_thread_reset_hit_rate (void);
static unsigned long long readcount(void);
static unsigned long long writecount(void);
 
static void syscall_handler (struct intr_frame *);
static void copy_in (void *, const void *, size_t);

 
void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}
 
/* System call handler. */
static void
syscall_handler (struct intr_frame *f) 
{
  typedef int syscall_function (int, int, int);

  /* A system call. */
  struct syscall 
    {
      size_t arg_cnt;           /* Number of arguments. */
      syscall_function *func;   /* Implementation. */
    };

  /* Table of system calls. */
  static const struct syscall syscall_table[] =
    {
      {0, (syscall_function *) sys_halt},
      {1, (syscall_function *) sys_exit},
      {1, (syscall_function *) sys_exec},
      {1, (syscall_function *) sys_wait},
      {2, (syscall_function *) sys_create},
      {1, (syscall_function *) sys_remove},
      {1, (syscall_function *) sys_open},
      {1, (syscall_function *) sys_filesize},
      {3, (syscall_function *) sys_read},
      {3, (syscall_function *) sys_write},
      {2, (syscall_function *) sys_seek},
      {1, (syscall_function *) sys_tell},
      {1, (syscall_function *) sys_close},
      {1, (syscall_function *) sys_practice},
      {1, (syscall_function *) NULL}, /* mmap */
      {1, (syscall_function *) NULL},  /* munmap */
      {1, (syscall_function *) sys_chdir},  /* chdir */
      {1, (syscall_function *) sys_mkdir},  /* mkdir */
      {2, (syscall_function *) sys_readdir},  /* readdir */
      {1, (syscall_function *) sys_isdir},  /* isdir */
      {1, (syscall_function *) sys_inumber},
      {0, (syscall_function *) sys_cache_flush},
      {0, (syscall_function *) sys_get_thread_cache_accesses},
      {0, (syscall_function *) sys_get_thread_hits},
      {0, (syscall_function *) sys_thread_reset_hit_rate},
      {0, (syscall_function *) readcount},
      {0, (syscall_function *) writecount}
    };

  const struct syscall *sc;
  unsigned call_nr;
  int args[3];

  /* Get the system call. */
  copy_in (&call_nr, f->esp, sizeof call_nr);
  if (call_nr >= sizeof syscall_table / sizeof *syscall_table)
    thread_exit ();
  sc = syscall_table + call_nr;

  /* Get the system call arguments. */
  ASSERT (sc->arg_cnt <= sizeof args / sizeof *args);
  memset (args, 0, sizeof args);
  copy_in (args, (uint32_t *) f->esp + 1, sizeof *args * sc->arg_cnt);

  /* Execute the system call,
     and set the return value. */
  f->eax = sc->func (args[0], args[1], args[2]);
}
 
/* Returns true if UADDR is a valid, mapped user address,
   false otherwise. */
static bool
verify_user (const void *uaddr) 
{
  return (uaddr < PHYS_BASE
          && pagedir_get_page (thread_current ()->pagedir, uaddr) != NULL);
}
 
/* Copies a byte from user address USRC to kernel address DST.
   USRC must be below PHYS_BASE.
   Returns true if successful, false if a segfault occurred. */
static inline bool
get_user (uint8_t *dst, const uint8_t *usrc)
{
  int eax;
  asm ("movl $1f, %%eax; movb %2, %%al; movb %%al, %0; 1:"
       : "=m" (*dst), "=&a" (eax) : "m" (*usrc));
  return eax != 0;
}
 
/* Writes BYTE to user address UDST.
   UDST must be below PHYS_BASE.
   Returns true if successful, false if a segfault occurred. */
static inline bool
put_user (uint8_t *udst, uint8_t byte)
{
  int eax;
  asm ("movl $1f, %%eax; movb %b2, %0; 1:"
       : "=m" (*udst), "=&a" (eax) : "q" (byte));
  return eax != 0;
}
 
/* Copies SIZE bytes from user address USRC to kernel address
   DST.
   Call thread_exit() if any of the user accesses are invalid. */
static void
copy_in (void *dst_, const void *usrc_, size_t size) 
{
  uint8_t *dst = dst_;
  const uint8_t *usrc = usrc_;
 
  for (; size > 0; size--, dst++, usrc++) 
    if (usrc >= (uint8_t *) PHYS_BASE || !get_user (dst, usrc)) 
      thread_exit ();
}
 
/* Creates a copy of user string US in kernel memory
   and returns it as a page that must be freed with
   palloc_free_page().
   Truncates the string at PGSIZE bytes in size.
   Call thread_exit() if any of the user accesses are invalid. */
static char *
copy_in_string (const char *us) 
{
  char *ks;
  size_t length;
 
  ks = palloc_get_page (0);
  if (ks == NULL) 
    thread_exit ();
 
  for (length = 0; length < PGSIZE; length++)
    {
      if (us >= (char *) PHYS_BASE || !get_user (ks + length, us++)) 
        {
          palloc_free_page (ks);
          thread_exit (); 
        }
       
      if (ks[length] == '\0')
        return ks;
    }
  ks[PGSIZE - 1] = '\0';
  return ks;
}
 
/* Practice system call. */
static int
sys_practice (int x)
{
  return x + 1;
}

/* Halt system call. */
static int
sys_halt (void)
{
  /* Flush the cache */
  cache_flush (); 
  shutdown_power_off ();
}
 
/* Exit system call. */
static int
sys_exit (int exit_code) 
{
  cache_flush (); 
  thread_current ()->wait_status->exit_code = exit_code;
  thread_exit ();
  NOT_REACHED ();
}
 
/* Exec system call. */
static int
sys_exec (const char *ufile) 
{
  tid_t tid;
  char *kfile = copy_in_string (ufile);
 
  tid = process_execute (kfile);
 
  palloc_free_page (kfile);
 
  return tid;
}
 
/* Wait system call. */
static int
sys_wait (tid_t child) 
{
  return process_wait (child);
}
 
/* Create system call. */
static int
sys_create (const char *ufile, unsigned initial_size) 
{
  char *kfile = copy_in_string (ufile);
  bool ok;
   
  ok = filesys_create (kfile, initial_size);
 
  palloc_free_page (kfile);
 
  return ok;
}
 
/* Remove system call. */
// TODO
static int
sys_remove (const char *ufile) 
{
  char *kfile = copy_in_string (ufile);
  bool ok;

  struct inode *i = resolve_pathname (kfile);
  if (i == NULL)  // error
    return 0;
  if (inode_is_dir (i)) // directory
  { 
    struct dir *dir = dir_open (i);
    char name[15];
    if (dir_readdir(dir, name)) 
    {
      dir_close(dir);
      return 0;
    }
    dir_close(dir);
  }

  ok = filesys_remove (kfile);
 
  palloc_free_page (kfile);
 
  return ok;
}
 
/* A file descriptor, for binding a file handle to a file. */
struct file_descriptor
  {
    struct list_elem elem;      /* List element. */
    struct file *file;          /* File. */
    struct dir *dir;            /* Directory */
    int handle;                 /* File handle. */
    bool is_dir;                /* True if this is a directory, false if file */
  };
 
/* Open system call. */
static int
sys_open (const char *ufile) 
{
  char *kfile = copy_in_string (ufile);
  struct file_descriptor *fd;
  int handle = -1;
  fd = malloc (sizeof *fd);
  if (fd != NULL)
    {
      struct inode *i = resolve_pathname (kfile);
      if (i == NULL)  // error
      {
        palloc_free_page (kfile);
        return -1;
      }
      if (inode_is_dir (i)) // directory
      {
        fd->dir = dir_open (i);
        fd->is_dir = true;
        if (fd->dir != NULL)
        {
          struct thread *cur = thread_current ();
          handle = fd->handle = cur->next_handle++;
          list_push_front (&cur->fds, &fd->elem);
        }
        else 
          {
            free (fd);
          }
      }
      else //file
      {
        fd->file = file_open (i);
        fd->is_dir = false;
        if (fd->file != NULL)
          {
            struct thread *cur = thread_current ();
            handle = fd->handle = cur->next_handle++;
            list_push_front (&cur->fds, &fd->elem);
          }
        else 
          {
            free (fd);
          }
      }
    }
  
  palloc_free_page (kfile);
  return handle;
}
 
/* Returns the file descriptor associated with the given handle.
   Terminates the process if HANDLE is not associated with an
   open file. */
static struct file_descriptor *
lookup_fd (int handle) 
{
  struct thread *cur = thread_current ();
  struct list_elem *e;
   
  for (e = list_begin (&cur->fds); e != list_end (&cur->fds);
       e = list_next (e))
    {
      struct file_descriptor *fd;
      fd = list_entry (e, struct file_descriptor, elem);
      if (fd->handle == handle)
        return fd;
    }
 
  thread_exit ();
}
 
/* Filesize system call. */
static int
sys_filesize (int handle) 
{
  struct file_descriptor *fd = lookup_fd (handle);
  int size;
 
  size = file_length (fd->file);
 
  return size;
}
 
/* Read system call. */
static int
sys_read (int handle, void *udst_, unsigned size) 
{
  uint8_t *udst = udst_;
  struct file_descriptor *fd;
  int bytes_read = 0;

  /* Handle keyboard reads. */
  if (handle == STDIN_FILENO) 
    {
      for (bytes_read = 0; (size_t) bytes_read < size; bytes_read++)
        if (udst >= (uint8_t *) PHYS_BASE || !put_user (udst++, input_getc ()))
          thread_exit ();
      return bytes_read;
    }

  /* Handle all other reads. */
  fd = lookup_fd (handle);
  if (fd->is_dir) 
    return -1;
  while (size > 0) 
    {
      /* How much to read into this page? */
      size_t page_left = PGSIZE - pg_ofs (udst);
      size_t read_amt = size < page_left ? size : page_left;
      off_t retval;

      /* Check that touching this page is okay. */
      if (!verify_user (udst)) 
        {
          thread_exit ();
        }

      /* Read from file into page. */
      retval = file_read (fd->file, udst, read_amt);
      if (retval < 0)
        {
          if (bytes_read == 0)
            bytes_read = -1; 
          break;
        }
      bytes_read += retval;

      /* If it was a short read we're done. */
      if (retval != (off_t) read_amt)
        break;

      /* Advance. */
      udst += retval;
      size -= retval;
    }
   
  return bytes_read;
}
 
/* Write system call. */
static int
sys_write (int handle, void *usrc_, unsigned size) 
{
  uint8_t *usrc = usrc_;
  struct file_descriptor *fd = NULL;
  int bytes_written = 0;

  /* Lookup up file descriptor. */
  if (handle != STDOUT_FILENO)
  {
    fd = lookup_fd (handle);
    if (fd->is_dir)
      return -1;
  }
  
  while (size > 0) 
    {
      /* How much bytes to write to this page? */
      size_t page_left = PGSIZE - pg_ofs (usrc);
      size_t write_amt = size < page_left ? size : page_left;
      off_t retval;

      /* Check that we can touch this user page. */
      if (!verify_user (usrc)) 
        {
          thread_exit ();
        }

      /* Do the write. */
      if (handle == STDOUT_FILENO)
        {
          putbuf (usrc, write_amt);
          retval = write_amt;
        }
      else
        retval = file_write (fd->file, usrc, write_amt);
      if (retval < 0) 
        {
          if (bytes_written == 0)
            bytes_written = -1;
          break;
        }
      bytes_written += retval;

      /* If it was a short write we're done. */
      if (retval != (off_t) write_amt)
        break;

      /* Advance. */
      usrc += retval;
      size -= retval;
    }
 
  return bytes_written;
}
 
/* Seek system call. */
static int
sys_seek (int handle, unsigned position) 
{
  struct file_descriptor *fd = lookup_fd (handle);
   
  if ((off_t) position >= 0)
    file_seek (fd->file, position);
 
  return 0;
}
 
/* Tell system call. */
static int
sys_tell (int handle) 
{
  struct file_descriptor *fd = lookup_fd (handle);
  unsigned position;
   
  position = file_tell (fd->file);
 
  return position;
}
 
/* Close system call. */
static int
sys_close (int handle) 
{
  struct file_descriptor *fd = lookup_fd (handle);
  if (fd->is_dir) //directory
    dir_close (fd->dir);
  else
    file_close (fd->file);
  list_remove (&fd->elem);
  free (fd);
  return 0;
}
 
/* On thread exit, close all open files. */
void
syscall_exit (void) 
{
  struct thread *cur = thread_current ();
  struct list_elem *e, *next;
   
  for (e = list_begin (&cur->fds); e != list_end (&cur->fds); e = next)
    {
      struct file_descriptor *fd;
      fd = list_entry (e, struct file_descriptor, elem);
      next = list_next (e);
      if (fd->is_dir)
        dir_close(fd->dir);
      else
        file_close (fd->file);
      free (fd);
    }
}

static bool
sys_chdir (const char *dir)
{ 
  char *kdir = copy_in_string (dir);
      
  struct thread *cur = thread_current ();
  if (dir_is_removed(cur->cwd))
    {
      palloc_free_page (kdir);
      return false;
    }
  struct inode *i = resolve_pathname (kdir);
  if (i == NULL) 
    {
      palloc_free_page (kdir);
      return false;
    }
  
  dir_close(cur->cwd);
  struct dir *new_dir = dir_open (i); 
  cur->cwd = new_dir;
  
  palloc_free_page (kdir);
  
  return true;
}

static bool
sys_mkdir (const char *dir) 
{
  char *kdir = copy_in_string (dir);
  bool ok = filesys_create_dir (kdir, 16);
  palloc_free_page (kdir);
  return ok;
}

static bool
sys_readdir (int handle, char *name) 
{
  struct file_descriptor *fd = lookup_fd (handle);
  if (!fd->is_dir) 
  {
    return false;
  }
  return dir_readdir (fd->dir, name);
}

static bool 
sys_isdir (int handle)
{
  struct file_descriptor *fd = lookup_fd (handle);
  return fd->is_dir;
}

static int
sys_inumber (int handle)
{
  struct file_descriptor *fd = lookup_fd (handle);
  if (fd->is_dir)
    return dir_inumber (fd->dir);
  else
    return file_inumber (fd->file);
}

static void
sys_cache_flush (void)
{
  return cache_flush ();
}

static int
sys_get_thread_cache_accesses (void)
{
  struct thread *cur = thread_current ();
  return cur->num_cache_accesses;
}

static int
sys_get_thread_hits (void)
{
  struct thread *cur = thread_current ();
  return cur->num_hits;
}

static void
sys_thread_reset_hit_rate (void)
{
  struct thread *cur = thread_current ();
  cur->num_cache_accesses = 0;
  cur->num_hits = 0;
}

unsigned long long
readcount (void)   
{ 
  return get_readcnt(BLOCK_FILESYS);
}

unsigned long long
writecount (void) 
{
  return get_writecnt(BLOCK_FILESYS);
}
