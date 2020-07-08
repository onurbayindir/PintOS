/* Measures cache hit rate by
    - resetting buffer cache
    - Opening/reading a file while recording how many hits
    - Opening/reading same file again recording how many hits
    - Passes if hit count is higher the second time around */
#include <random.h>
#include <syscall.h>
#include "tests/lib.h"
#include "tests/main.h"
#include "threads/fixed-point.h"

static char buf[10240];
static const char file_name[] = "data";

void
test_main (void)
{ 
  int fd;

  int cache_accesses1;
  int hits1;

  int cache_accesses2;
  int hits2;

  /* Create and open a file */
  CHECK (create (file_name, sizeof (buf)), "create \"%s\"", file_name);
  CHECK ((fd = open (file_name)) > 1, "open \"%s\"", file_name);
  random_bytes (buf, sizeof buf);
  CHECK (write (fd, buf, sizeof buf) > 0, "write \"%s\"", file_name);

  /* reset buffer cache */
  cache_flush ();
  msg ("flushed cache");
  
  /* open and read try 1 */
  CHECK ((fd = open (file_name)) > 1, "open \"%s\"", file_name);
  CHECK (read (fd, buf, sizeof buf) == (int) sizeof buf, "read1");
  cache_accesses1 = thread_cache_accesses ();
  hits1 = thread_hits ();
  close (fd);

  thread_reset_hit_rate ();
  
  /* open and read try 2 */
  CHECK ((fd = open (file_name)) > 1, "open \"%s\"", file_name);
  CHECK (read (fd, buf, sizeof buf) == (int) sizeof buf, "read2");
  cache_accesses2 = thread_cache_accesses ();
  hits2 = thread_hits ();
  close (fd);
    
  /* verify higher hit_rate2 */
  //msg ("hit_rate1: %f", &hit_rate1);
  //msg ("hit_rate1: %f", &hit_rate2);
  fixed_point_t hit_rate1 = fix_frac (hits1, cache_accesses1);
  fixed_point_t hit_rate2 = fix_frac (hits2, cache_accesses2);
  CHECK (fix_compare (hit_rate2, hit_rate1), "check hit_rate2 > hit_rate1");
}
