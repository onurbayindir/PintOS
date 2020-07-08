#include <syscall.h>
#include <random.h>

#include "tests/lib.h"
#include "tests/main.h"
 

 // come to syscall.c im following u go  thee nice
const unsigned size = 65536;
const char buf[65536]; 

static const char file_name[] = "data";

void
test_main (void)
{

  int fd;

  /* Create and open a 64kB file */
  CHECK (create (file_name, size), "create \"%s\"", file_name);
  CHECK ((fd = open (file_name)) > 1, "open \"%s\"", file_name);
  random_bytes (buf, sizeof buf);

  /* Write byte by byte */
  int bytes_written;
  CHECK (write (fd, buf, sizeof buf) > 0, "write \"%s\"", file_name);
  CHECK ((fd = open (file_name)) > 1, "open \"%s\"", file_name);
  CHECK (read (fd, buf, sizeof buf) == (int) sizeof buf, "read \"%s\"", file_name);
  
  CHECK (readcount () >= 128, "checking readcnt");
  CHECK (writecount() >= 128, "checking writecnt");
  close(fd);

 
}