![frog](/image1.png) 
![kubi](/image2.png)
*Find somebody who looks at you the way the dude on the right looks at Kubi*

Final Report for Project 1: User Programs
=========================================
### Changes to Task 1
* We simplified the design from the design doc and did not save the arguments to a Pintos list, since that was unnecessary, and instead we directly pushed them on the stack after validation. 
* We also included some small details mentioned in design review, such as aligning the stack to 16 bytes and setting the number of bytes in an address to 4.
* We also realized we needed to ensure that the filename argument passed into thread_create was the first filename and strip out all the following arguments.
* We also increased the allowed arguments quantity from LOADER_ARGS_LEN to a larger amount to account for test cases like args_many.

### Changes to Task 2
* We did not use a queue for the syscalls, as that would be handled for us. 
* As Annie noted during our design review, we did not end up using `pagedir_create` or `pagedir_destroy`. We clarified during the design review that these were on the document for reference only.
* We made a few changes to the `wait_status` struct:
  * We added a lock to synchronize changes to `ref_cnt` in the parent-child `wait_status` struct. 
  * We used a semaphore and a boolean to inform the parent of whether the child finished loading.
    * Once the child’s load finishes (or fails), the child updates `wait_status->succeeded` and ups its semaphore, informing the parent of its status and allowing exec to return with the proper value.
    * The semaphore also doubles as the way for a parent to wait on its child, where the child downs the semaphore until it has finished running.
* We also needed the parent to point to its own `wait_status` struct, so we included that in the thread struct.
* Instead of using a global list of wait_status structs, we altered `struct thread` so that each thread carries a `struct list children` of its children’s `wait_status`es.
* In process_exit, we added more robust clean-up logic to free `malloc`’d structures and close files opened by the thread in question.
* In order to share a `wait_status` object between a parent and its child we passed a pointer to a struct called `two_args` containing pointers to both the start_process arguments and the `wait_status` struct (initialized in `process_execute()`) to `thread_create()`.

### Changes to Task 3
  * We tweaked the details of ROX from the design doc by saving the executable file pointer to the thread struct after we open the file in `load()`, and we call `file_deny_write()` on the file. We close this file when the process exits.
  * We added thread id as a tid attribute to file_desc to check the thread and prevent file_close from closing other threads. 
  * We created helper functions to validate every single argument that has been passed in to avoid any possible harm on the system.
  * We created a unique linked list of fd’s for each thread.
  * We implemented the functions that have been provided in filesys.c and file.c files under filesys folder.
  * We added a lock to the file struct before calling any functions from filesys to avoid processing on the same file at the same time.

## Who Did What?
### Task 1 Credits
Lead Developer - Daniel Sun  
Regional Developer - Daniel Sun  
Associate to the Regional Developer - Daniel Sun  
All Other Developers - Daniel Sun  
Supporting Developer - Daniel Sun  
Development Director - Daniel Sun  
Music Composer - Daniel Sun  
Costume Designer - Daniel Sun  
Executive Producer - Crystal Jin  
Associate Producers - Daniel Sun, Nintendo DS, D-day, Big D, Sunny-D, Vitamin D, Sun-Day  
### Task 2 Credits
Michael and Crystal collaborated to complete task 2.
### Task 3 Credits
Onur and Daniel collaborated to complete task 3.

## Reflecting on Our Spiritual Journey Together
### What Went Well?
We met regularly and collaborated effectively to finish the project on time :O
Regular, in-person meetings helped us stay on top of work and communicate about what was or wasn't working. This way, we managed to mostly avoid incompatibilities and merge conflicts between our local versions of the project. Also, we managed to strike a good balance between division of labor and pair-programming on more challenging problems.
### What Could Have Gone Better?
We were working till the last day to finish our code. Next time we’ll manage our time better so that doesn’t happen (maybe) (hopefully)
In addition, we occasionally had moments where merge conflicts did happen. Some of these were inevitable, but others could have been avoided by pulling more frequently. We will encourage each other to do so in the future. Now that everyone is ramped up on C and various other tools like git and gdb, we anticipate even smoother collaboration in projects to come.
