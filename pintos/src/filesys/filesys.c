#include "filesys/filesys.h"
#include <debug.h>
#include <stdio.h>
#include <string.h>
#include "filesys/file.h"
#include "filesys/free-map.h"
#include "filesys/inode.h"
#include "filesys/directory.h"
#include "filesys/cache.h"
#include "threads/thread.h"
#include "threads/malloc.h"

/* Partition that contains the file system. */
struct block *fs_device;

static void do_format (void);

/* Initializes the file system module.
   If FORMAT is true, reformats the file system. */
void
filesys_init (bool format) 
{
  fs_device = block_get_role (BLOCK_FILESYS);
  if (fs_device == NULL)
    PANIC ("No file system device found, can't initialize file system.");

  inode_init ();
  free_map_init ();
  cache_init ();

  if (format) 
    do_format ();

  free_map_open ();
}

/* Shuts down the file system module, writing any unwritten data
   to disk. */
void
filesys_done (void) 
{
  free_map_close ();
}

/* Extracts a file name part from *SRCP into PART, and updates *SRCP so that the
next call will return the next file name part. Returns 1 if successful, 0 at
end of string, -1 for a too-long file name part. */
static int
get_next_part (char *part, const char **srcp) 
{
  const char *src = *srcp;
  char *dst = part;
  /* Skip leading slashes. If it's all slashes, we're done. */
  while (*src == '/')
    src++;
  if (*src == '\0')
    return 0;
  /* Copy up to NAME_MAX character from SRC to DST. Add null terminator. */
  while (*src != '/' && *src != '\0') {
    if (dst < part + NAME_MAX)
      *dst++ = *src;
    else
      return -1;
    src++;
  }
  *dst = '\0';
  /* Advance source pointer. */
  *srcp = src;
  return 1;
}

/* Returns the top level directory (root if absolute, cwd if relative) */
struct dir*
absolute_or_relative (const char *name) 
{
  struct dir *d;
  if (name[0] == '/') {  // absolute path
  {
    d = dir_open_root ();
  }
  } else {  // relative path
    d = dir_reopen (thread_current ()->cwd);
  }
  return d;
}

/* Returns inode corresponding to pathname */
struct inode *
resolve_pathname (const char *name) 
{
  if (strlen(name) == 0) 
    return NULL;
  struct dir *d = absolute_or_relative (name);
  if (d == NULL) 
    return NULL;
  char *part = malloc (NAME_MAX + 1);
  if (part == NULL)
  {
    dir_close(d);
    return NULL;
  }
  char *rest = (char*) malloc (strlen(name) + 1);
  const char *rest_og = rest;
  if (rest == NULL) {
    dir_close(d);
    free (part);
    return NULL;
  }
  strlcpy (rest, name, strlen(name) + 1);
  int success = 1;
  struct inode *i = dir_get_inode (d);

  while (success > 0) {
    success = get_next_part (part, &rest);
    if (success == -1) { //file part too long
      dir_close(d);
      free (part);
      free (rest_og);
      return NULL;
    } else if (success == 0) {
      break;
    }

    dir_lookup (d, part, &i);
    if (i == NULL) {
      dir_close(d);
      free (part);
      free (rest_og);
      return NULL;
    }
    dir_close (d);
    d = dir_open (i);
  }
  i = inode_reopen(i);
  dir_close(d);
  free (part);
  free (rest_og);
  return i;
}

/* Gets the name of the file or the directory to create
   and saves lowest level directory into dir */
struct dir *
get_name_and_directory (const char *pathname, char name[NAME_MAX + 1]) 
{
  int end = strlen(pathname) - 1;
  /* Get rid of any leading slashes */
  while (pathname[end] == '/' && end >= 0)
    end--;
  int beg = end;
  if (beg == -1) return NULL;
  while (pathname[beg] != '/' && beg >= 0)
    beg--;
  if (end - beg + 1 > NAME_MAX + 1)
    return NULL;
  if (beg <= 0)
  {
    if (beg == 0) // create file in root directory
      strlcpy(name, pathname + 1, end - beg + 1);
    else // create file in current directory
      strlcpy(name, pathname, end - beg + 1);
    return absolute_or_relative (pathname);
  }
  char *parent_dir = malloc (beg + 1);
  strlcpy (parent_dir, pathname, beg + 1);
  strlcpy (name, pathname + beg + 1, end - beg + 1);

  struct inode *inode = resolve_pathname (parent_dir);
  free(parent_dir);
  return dir_open(inode);
}

/* Creates a file named NAME with the given INITIAL_SIZE.
   Returns true if successful, false otherwise.
   Fails if a file named NAME already exists,
   or if internal memory allocation fails. */
bool
filesys_create (const char *name, off_t initial_size)
{
  block_sector_t inode_sector = 0;
  char* file_name = malloc(NAME_MAX + 1);
  struct dir *dir = get_name_and_directory (name, file_name);
  bool success = (dir != NULL
                  && free_map_allocate (1, &inode_sector)
                  && inode_create (inode_sector, initial_size)
                  && dir_add (dir, file_name, inode_sector));
  if (!success && inode_sector != 0)
    free_map_release (inode_sector, 1);
  dir_close (dir);
  free(file_name);

  return success;
}

/* Creates a directory named NAME with the given INITIAL_SIZE.
   Returns true if successful, false otherwise.
   Fails if a file named NAME already exists,
   or if internal memory allocation fails. */
bool
filesys_create_dir (const char *name, off_t initial_size) 
{
  block_sector_t inode_sector = 0; 
  char* dir_name = malloc(NAME_MAX + 1);
  struct dir *dir = get_name_and_directory (name, dir_name);
  bool success = (dir != NULL
                  && free_map_allocate (1, &inode_sector)
                  && dir_create(inode_sector, initial_size) 
                  && dir_add (dir, dir_name, inode_sector)); 
  if (!success && inode_sector != 0)
    free_map_release (inode_sector, 1);
  dir_close (dir);
  free(dir_name);

  return success;
}

/* Opens the file with the given NAME.
   Returns the new file if successful or a null pointer
   otherwise.
   Fails if no file named NAME exists,
   or if an internal memory allocation fails. */
struct file *
filesys_open (const char *name)
{
  char* file_name = malloc(NAME_MAX + 1);
  struct dir *dir = get_name_and_directory (name, file_name);
  struct inode *inode = NULL;
  if (dir != NULL)
  {
    dir_lookup (dir, file_name, &inode);
    dir_close (dir);
    if (inode == NULL) 
    {
      return NULL;
    }
  }

  return file_open (inode);
}

/* Deletes the file named NAME.
   Returns true if successful, false on failure.
   Fails if no file named NAME exists,
   or if an internal memory allocation fails. */
bool
filesys_remove (const char *name) 
{
  char* file_name = malloc(NAME_MAX + 1);
  struct dir *dir = get_name_and_directory (name, file_name);
  bool success = dir != NULL && dir_remove (dir, file_name);
  dir_close (dir); 
  free(file_name);

  return success;
}

/* Formats the file system. */
static void
do_format (void)
{
  printf ("Formatting file system...");
  free_map_create ();
  if (!dir_create (ROOT_DIR_SECTOR, 16))
    PANIC ("root directory creation failed");
  free_map_close ();
  printf ("done.\n");
}
