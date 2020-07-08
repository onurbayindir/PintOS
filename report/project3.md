Final Report for Project 3: File Systems
========================================

### Final Report III: Revenge of the Sys
## Changes to the Initial Design Document
The changes you made since your initial design document and why you made them (feel free to re-iterate what you discussed with your TA in the design review) 

“You were the Chosen One! It was said that you would destroy race conditions, not create them! Bring balance to the Filesystem, not leave it in deadlock!”
~ Annie Ro

Part 1
* We did not add a new cache.c file, so we did that and wrote our cache functions in that.
* We added a per entry lock in addition to the global cache lock.
* We forgot to include a valid bit in the design doc, which we added to the struct.
* We flushed the cache on shutdown for persistence tests.
* We also chose to implement an API for our cache, modeled after the `block_read` and `block_write` API. This was also a suggestion on Annie’s part.
* We originally kept the inode_disk member of our struct inode intact in our design doc, but we have since removed it so as not to violate the 64 block limit on the cache.


Part 2
* We failed to consider 5 of the 6 synchronization situations that Annie kindly suggested.
* As we discussed with Annie during our design review, we added the following synchronization elements to our file extension design:  
A per-inode file growth lock  
A per-inode monitor to synchronize denied writes and `file_deny_write`  
A metadata lock for in-memory inode data  
A lock to synchronize in-memory inodes  

Part 3
* The entire directory tree structure was abandoned in favor of a series of `dir_lookup` calls. This meant that we had to reconsider how to do many of the required functions. 
* We added . and .. directories upon directory creation.
* We did pathname resolution directly in `filesys.c` by writing helper functions to determine if a path was relative or absolute and to return a reopened inode corresponding to the directory or file that the path references. We called those any time we required pathname resolution.

## A Reflection on the Project
#What exactly did each member do? 
* Everyone collaborated and worked extensively on Tasks 1,2, and 3, in addition to debugging. Most of the initial implementation was done in concert via video chat and a shared coding environment (VScode liveshare).
* Michael and Crystal debugged problematic test cases and put the finishing touches on the code.
* Daniel and Onur wrote the extra tests for the buffer cache.

#What went well?
* VScode liveshare worked well as a shared coding platform, except when it didn’t
* We worked together to discover that the real autograder was the friends we made along the way

#What could be improved? 
* The global health situation could really stand to be improved, as it prevented us from meeting in person and seriously cramped our style.
* We started late on the design doc and ended up not spending enough time on synchronization.

## Student Testing Report
### Test Case 1: Buffer Hit Rate
#### Provide a description of the feature your test case is supposed to test. Provide an overview of how the mechanics of your test case work, as well as a qualitative description of the expected output. 

The ultimate goal of this test was to ensure that our buffer cache was providing the efficiency we desired, by ensuring that reading a file the first time around would be objectively faster than the second read. To measure this, we read the same file (a large file) twice, and ensured that the second time around, the hit rate significantly increases.
One interesting quirk about this test was that in our implementation, it required the use of the fixed-point library, since we had to compare two decimal values (hit rates). We also had to make sure we flushed the cache before we started the 2 reads, since we needed to make sure we were measuring from a blank cache slate.  
Lo and behold,  
For test file size 10240 B, hit rate for read 1 was 63/85 = 74% and hit rate for read 2 was 100%.  
For test file size 51200 B, hit rate for read 1 was 252/355 = 71% and hit rate for read 2 was 253/355 = 71%.  

#### Provide the output of your own Pintos kernel when you run the test case. Please copy the full raw output file from filesys/build/tests/filesys/extended/your-test-1.output as well as the raw results from filesys/build/tests/filesys/extended/your-test-1.result. 

buf-hit-rate.output:<br />
Copying tests/filesys/extended/tar to scratch partition...<br />
qemu-system-i386 -device isa-debug-exit -hda /tmp/Y08Ooc4EDr.dsk -hdb tmp.dsk -m 4 -net none -nographic -monitor null<br />
PiLo hda1^M<br />
Loading............^M<br />
Kernel command line: -q -f extract run buf-hit-rate<br />
Pintos booting with 3,968 kB RAM...<br />
367 pages available in kernel pool.<br />
367 pages available in user pool.<br />
Calibrating timer...  377,241,600 loops/s.<br />
hda: 1,008 sectors (504 kB), model "QM00001", serial "QEMU HARDDISK"<br />
hda1: 193 sectors (96 kB), Pintos OS kernel (20)<br />
hda2: 243 sectors (121 kB), Pintos scratch (22)<br />
hdb: 5,040 sectors (2 MB), model "QM00002", serial "QEMU HARDDISK"<br />
hdb1: 4,096 sectors (2 MB), Pintos file system (21)<br />
filesys: using hdb1<br />
scratch: using hda2<br />
Formatting file system...done.<br />
Boot complete.<br />
Extracting ustar archive from scratch device into file system...<br />
Putting 'buf-hit-rate' into the file system...<br />
Putting 'tar' into the file system...<br />
Erasing ustar archive...<br />
Executing 'buf-hit-rate':<br />
(buf-hit-rate) begin<br />
(buf-hit-rate) create "data"<br />
(buf-hit-rate) open "data"<br />
(buf-hit-rate) write "data"<br />
(buf-hit-rate) flushed cache<br />
(buf-hit-rate) open "data"<br />
(buf-hit-rate) read1<br />
(buf-hit-rate) open "data"<br />
(buf-hit-rate) read2<br />
(buf-hit-rate) check hit_rate2 > hit_rate1<br />
(buf-hit-rate) end<br />
buf-hit-rate: exit(0)<br />
Execution of 'buf-hit-rate' complete.<br />
Timer: 87 ticks<br />
Thread: 17 idle ticks, 66 kernel ticks, 4 user ticks<br />
hdb1 (filesys): 61 reads, 511 writes<br />
hda2 (scratch): 242 reads, 2 writes<br />
Console: 1267 characters output<br />
Keyboard: 0 keys pressed<br />
Exception: 0 page faults<br />
Powering off...<br />

buf-hit-rate.result:<br />
PASS<br />

#### Identify two non-trivial potential kernel bugs, and explain how they would have affected your output of this test case. You should express these in this form: “If your kernel did X instead of Y, then the test case would output Z instead.”. 

* One bug that this test actually revealed was that we weren’t invalidating cache entries when flushing the cache. In this test, before reading a large file twice, we flush the cache so it can start with a blank slate again. However, since our kernel wasn’t invalidating cache entries, the test case originally outputted a hit rate of 100% for both first and second reads.  

* Another bug that this test could reveal is any bug that would cause us to conditionally not access the cache when we are supposed to. For example, during a `dir_lookup` call in path resolution, the `struct dir` passed in could be corrupted or invalid. In this case, `dir_lookup` would fail to return a valid inode, and since `lookup` calls `inode_read_at`, an invalid `struct dir` may cause us to read the inode more or fewer times. In this case, our test case may output different cache access amounts for each read of the same file, which could also cause incorrect or inconsistent hit rates.

### Test Case 2: Buffer Coalescing of Writes
#### Provide a description of the feature your test case is supposed to test. Provide an overview of how the mechanics of your test case work, as well as a qualitative description of the expected output. 

The ultimate goal for this test was checking the block devices in the implemented cache system. The inputted file size is at least 64kB and each block size is defined as 512 bytes so in this test case we are assuring that we have at least 128 block devices occupied to handle such large files.

#Provide the output of your own Pintos kernel when you run the test case. Please copy the full raw output file from filesys/build/tests/filesys/extended/your-test-1.output as well as the raw results from filesys/build/tests/filesys/extended/your-test-1.result. 

buffer-counter.output:<br />
Copying tests/filesys/extended/buffer-counter to scratch partition...<br />
Copying tests/filesys/extended/tar to scratch partition...<br />
qemu-system-i386 -device isa-debug-exit -hda /tmp/PXuwDu_NJe.dsk -hdb tmp.dsk -m 4 -net none -nographic -monitor null<br />
PiLo hda1^M<br />
Loading............^M<br />
Kernel command line: -q -f extract run buffer-counter<br />
Pintos booting with 3,968 kB RAM...<br />
367 pages available in kernel pool.<br />
367 pages available in user pool.<br />
Calibrating timer...  209,305,600 loops/s.<br />
hda: 1,008 sectors (504 kB), model "QM00001", serial "QEMU HARDDISK"<br />
hda1: 193 sectors (96 kB), Pintos OS kernel (20)<br />
hda2: 241 sectors (120 kB), Pintos scratch (22)<br />
hdb: 5,040 sectors (2 MB), model "QM00002", serial "QEMU HARDDISK"<br />
hdb1: 4,096 sectors (2 MB), Pintos file system (21)<br />
filesys: using hdb1<br />
scratch: using hda2<br />
Formatting file system...done.<br />
Boot complete.<br />
Extracting ustar archive from scratch device into file system...<br />
Putting 'buffer-counter' into the file system...<br />
Putting 'tar' into the file system...<br />
Erasing ustar archive...<br />
Executing 'buffer-counter':<br />
(buffer-counter) begin<br />
(buffer-counter) create "data"<br />
(buffer-counter) open "data"<br />
(buffer-counter) write "data"<br />
(buffer-counter) open "data"<br />
(buffer-counter) read "data"<br />
(buffer-counter) checking readcnt<br />
(buffer-counter) checking writecnt<br />
(buffer-counter) end<br />
buffer-counter: exit(0)<br />
Execution of 'buffer-counter' complete.<br />
Timer: 105 ticks<br />
Thread: 34 idle ticks, 64 kernel ticks, 7 user ticks<br />
hdb1 (filesys): 171 reads, 744 writes<br />
hda2 (scratch): 240 reads, 2 writes<br />
Console: 1248 characters output<br />
Keyboard: 0 keys pressed<br />
Exception: 0 page faults<br />
Powering off...<br />

buffer-counter.result:<br />
PASS<br />

#### Identify two non-trivial potential kernel bugs, and explain how they would have affected your output of this test case. You should express these in this form: “If your kernel did X instead of Y, then the test case would output Z instead.”. 

* One of the potential bugs for this case is if the kernel imports the same .c file more than once during the function calls, then the test case will give duplicate function definition errors. To avoid such issues you could change .c imports with .h and to reach some of the properties in the struct that is defined in the .c file you can use helper get functions.

* The other potential bug for this case since we are working with large files if the kernel timer is not long enough to handle such file it might give timeout error before it successfully writes and reads the file.

#### Tell us about your experience writing tests for Pintos. What can be improved about the Pintos testing system? (There’s a lot of room for improvement.) What did you learn from writing test cases?

Writing test cases revealed how useful syscalls can be. In our virtual OS environment, literally any information from the kernel can be requested just by invoking or creating a corresponding syscall. One improvement could be to improve interfaces for printing floating/fixed point numbers, as it was an immense hurdle for such a seemingly small task.


