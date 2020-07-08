Design Document for Project 1: User Programs
============================================

## Group Members

* Crystal Jin <crystalj@berkeley.edu>
* Daniel Sun <daniel.s@berkeley.edu>
* Michael Wang <mywang@berkeley.edu>
* Onur Bayindir <onurbayindir@berkeley.edu>

# Project 1 Design Document
## Task 1: Argument Passing
### Data structures and functions
```
struct argument {
    struct list_elem elem;
    char* arg;
};
```
We can store the arguments in a Pintos linked list using this struct that contains a pointer to the argument string.
Global variable for address size

`int ADDRESS_BYTES = 8;`

This is a global variable to keep track of the number of bytes in an address. This is to prevent hardcoding when calculating how many bytes we will push onto the stack.

`int MAX_ARGS = 10;`

This is a global variable that defines a limit for the number of arguments that can be passed in the command line. Again, this is to prevent hardcoding.

### Algorithms
#### Parsing arguments

We will first need to process the string passed in and separate them into different arguments. We can do this through a helper function that parses the string and extracts the separate arguments, and returns them as a Pintos linked list. We can parse the string using the function `strtok_r`, using space as a delimiter, in which we can get the full arguments as their own strings. We then store the string in the heap using malloc, and add a new element to the Pintos list containing the address of the argument that we just stored. We repeat this process `MAX_ARGS` times, each time adding a new element containing an argument to the end of the Pintos list. For the padding, we can also have a variable arg_bytes to keep track of the total number of bytes we will be pushing onto the stack, and each time we obtain a new argument from the string, we use the function sizeof to calculate the number of bytes in the new argument, and add the value to arg_bytes. After we are done parsing the string, we can add `arg_bytes % 16` bytes of padding as a new list element to the front of the list. The function returns the list containing the arguments and padding.

#### Edge case: size of arguments

We need to make sure the number of bytes we are pushing onto the stack does not exceed `LOADER_ARGS_LEN`, defined in loader.h. After we are done parsing the string and right before we return the list containing the arguments, we calculate `arg_bytes + padding size + ((length of list + 3) * ADDRESS_BYTES)`, which gives us the total number of bytes we are pushing onto the stack. This value includes the bytes of argument words and the padding, the addresses of the arguments and the null pointer sentinel, and `argv`, `argc`, and a fake return address. We can check to see if this value is at or below `LOADER_ARGS_LEN`. If it is not, we throw an error.

#### Pushing to stack

After we have the arguments in separate list elements, we can push them to the stack. We can again use a helper function to do this. This function will take in the linked list of arguments as well as the initial `esp`, which will be `&if_.esp` in start_process. We first initialize a `char**` array addresses of size argc, so that we can store the addresses of the arguments after we push them onto the stack. This can be local, since we are not passing it to any other function. Then we push the arguments from the list onto the stack in reverse order using `memset` and the `esp`. We start from the end of the doubly linked list, and after we push each element, we free the memory it took up in the heap, and also store each address in addresses starting from the end of the array (but skipping the padding). After we have traversed the linked list, we first push a `null` pointer sentinel, and then we can push the addresses of the arguments stored in addresses onto the stack in reverse order. Finally, we push argv, which is the address that we just pushed to, and then argc, which is the size of the linked list - 1 (excluding the padding), and then an integer for the dummy return address. We would call this function right before we call load in start_process.

#### Other edge cases

* No null terminator: The string we pass in possibly does not end in a null terminator. This can be a problem that will be handled later when reading the arguments. It may also be caught because the number of bytes we push to the stack ends up exceeding the limit, for which we have a check.
* File not found: The name of the function we pass in (`argv[0]`) is not found. This is handled in the load function.
* Multiple spaces: We may possibly have multiple spaces as a delimiter in between the arguments, which is still valid. `strtok_r` will handle this case and skip over all spaces.
* Too many arguments: We handle this by only going getting `MAX_ARGS` arguments.

### Synchronization

We will not need any shared resources across threads, since this procedure occurs when we initialize a new process, and every process has its own stack.

### Rationale

* Instead of pushing the arguments directly to the stack as we parse the string, we saved them first to a Pintos linked list. This complicates the code, but it is necessary to check for size and prevent a stack overflow.
* We used a Pintos linked list for ease of separating the arguments from the original string that was passed in.
* We kept the linked list and the addresses array in the same order as the arguments were when they were first passed in for clarity. We would need to push them in reverse order onto the stack, but it is not too difficult to parse them in reverse order, since the list is a doubly linked list, and the array is just adjacent data in which we can start from the last address and subtract from it.
* It is necessary to store each argument in the heap, since we must pass the linked list to multiple functions and therefore store a pointer to the argument string.
* The runtime for this part is still O(N), since we are just going through the arguments a few times.

## Task 2: Process Control Syscalls
### Relevant Files

* `lib/user/syscall.c`: 
Every syscall has a corresponding function in this file which calls syscallX where X is the #arguments the syscall takes in.
Prepares syscall arguments on the stack and handles the transfer to kernel mode.
At the call to `syscall_handler()`, the system call number is in the 32-bit word at the caller’s stack pointer, the first argument is in the 32-bit word at the next higher address, and so on. 
The caller’s stack pointer is accessible to `syscall_handler()` as the esp member of the struct `intr_frame` passed to it. (struct * `intr_frame` is on the kernel stack.)
* `intr_frame` is located in `interrupt.h`, and contains the interrupted thread’s saved registers
userprog/syscall.c: 
* `syscall_handler()` passes in a struct `intr_frame` which is where we access registers
- The 80x86 convention for function return values is to place them in the eax. register. System calls that return a value can do so by modifying the eax member of the struct `intr_frame`.
* `lib/syscall-nr.h`: This file defines the syscall numbers for each syscall.
* `userprog/pagedir.c`: This file is helper for dereferencing the user pointer and validating 	
* `pagedir_create()`: Creates a new page directory that has mappings for kernel virtual addresses, but none for user virtual addresses. So each address has a value that is above PHYS_BASE. If there is not adequate memory it returns null pointer
* `pagedir_destroy()`: Freeing all of the references in the pages and destroys the directory. Basically iterating through every page in the directory and free every virtual address.
- Each function inside the file is focusing on the ways to validate the given virtual address and handles invalid virtual addresses or null pointers
threads/vaddr.h 
- Includes functions like ptov and vtop which converts the physical address to virtual address or the way around. 
- Is_user_vaddr checks if the virtual address is users or not and is_kernel_vaddr checks if the virtual address is kernels or not
- It also includes functions to offset within a page or getting the virtual page number. Additionally, there are functions for rounding up to the nearest page boundary or rounding down. To check the limits of the given page.
### Data Structures and Functions 
#### Semaphores for Incoming Syscalls:
Queue that stores all syscalls from all threads at any given time. Whenever a thread calls a syscall, it checks the size of the queue. If the queue size is 0, then we can just execute the syscall immediately. Else, if the queue size is 1 or more, then we add the syscall to the queue, and let the thread fall asleep. 

`static struct semaphore sema_syscall;`

Reasoning Through Example: Take the following scenario.
* Thread A is running and calls a syscall.
* Kernel deals with thread A’s syscall.
* Halfway through the syscall, the scheduler assigns a new thread to run, Thread B.
* Thread B calls a syscall.
* Kernel never finishes Thread A’s syscall, and starts executing Thread B’s syscall.
The important idea is that we want to enforce order with syscalls, such that the order the kernel deals with each syscall is the same order as each thread called them. To accomplish this, we will create a semaphore variable in `syscall.c` that is initialized to 1. Whenever a thread wants to perform a syscall, it will call `sema_down(&sema_syscall)`. After the thread finishes from the syscall, it will call `sema_up(&sema_syscall)`. Within the semaphore functions, there is a queue that stores all the waiting threads in order of calling `sema_down()`. As a result, this can enforce the order that each syscall is called in. Also, by waiting for a positive semaphore while starting a syscall, this prevents new syscalls from running when previous syscalls have not finished yet. 

#### “Switch Statements”:

The `userprog/syscall.c` file is relatively straightforward. We will have a bunch of if statements that execute based off of the syscall ID passed in. `lib/user/syscall.c` sets up each syscall such that in `userprog/syscall.c`, if we define `*args = f->esp` where each element of args is a 32 bit word, then `args[0]` gives us the syscall ID, `args[1]` gives us the first argument to the syscall, `args[2]` gives us the second argument to the syscall, and so on. For example, `exit(5)` takes in one integer argument, so `args[1]` is 5. The contents of each if statement will fill in registers based off of the contents of the stack established in `intr_frame` (eg. `f->eax = args[1]`). Then, we call the appropriate functions in either `thread.c` or `process.c` (eg. call `process_wait()` for the WAIT syscall).

#### Within exec: List of child threads

`static struct list child_processes;`

Currently, based on `thread.c`, threads do not have a list of their child threads. To facilitate functions and syscalls that may need to iterate through child threads, we can create a list of child thread structs. Every time `process_execute()` is called and the child successfully loads the new process, we will add the child’s thread struct to the list of child_processes.

#### Wait/Exit Struct for Communication between Parent and Child:

The premise of the wait and exit syscalls is that there must be a channel of communication between the parent and child such that when the child exits, the parent knows of this and can respond to this exit. To do this, we will create a wait_status struct that exists for each parent<->child relationship. The wait_status struct is created as soon as a parent creates a child.
The wait_status struct will contain the following contents:
* struct list_elem elem
- This will be used so that the parent can have a list of wait statuses for all of the children it is waiting on, denoted as a struct list wait_status_l. Whenever a new child is created, a new wait_status will be added to the parent’s list.
* tid_t tid 
- Whenever a parent calls wait on a child, it will iterate through its children list and find the child with TID tid. As we can see here, it is possible for the parent to wait on a NULL child, or a thread that is not its own child. This check is necessary. Then, it will create a wait_status between itself and the child.
* int exit_code
- When a child exits, the child will store its exit_code here to communicate that value with the parent.
* int file_descriptor
- This will facilitate file related function described more in detail in Task 3.

The remaining wait_status struct elements are based off of synchronization between parent and child processes. The actual mechanisms and purpose of synchronization are described in the synchronization section below.

### Algorithms
#### Validating Arguments before Executing Syscall

Within each case/if statement in `userprog/syscall.c`, arguments will be validated for the following invalid cases.
Invalid Cases -> Terminate user process if user program provides a pointer argument that:
* Is `NULL`
* Points to unmapped virtual address
- Call pagedir_get_page passing in thread_current->pagedir and the pointer address and check if this returns NULL.
- This function effectively takes the current thread’s page table, and returns what is the physical address of the page table’s virtual address.
* Points to kernel address space
- Call is_kernel_vaddr(), and check if this returns false and if is_user_vaddr() returns true. These two function calls are straightforward, they simply check the address’s relation to PHYS_BASE.
* Some syscall arguments are pointers to buffers inside the user process’s address space. Those buffer pointers could be invalid as well.

#### Edge Cases:

* For memory that is more than 1 byte (typically 4 bytes), check each byte for validity. This means we need to index into each byte.
- This can be accomplished by calling pagedir_get_page with thread_current->pagedir as before, but passing in the address shifted by 1 byte for all 3 possible shifts.

* Syscall number is invalid. This can simply be dealt with by adding an else case at the end of userlib.syscall.c. In this event, we will respond the same way we respond to invalid user arguments; we will simply terminate the user process by calling `process_exit()`.

#### Implementing Practice 

For this simple piece, within the PRACTICE case in userprog/syscall.c, we will expect the integer to be in args[1], and so we can simply call the function process_practice(args[1]) and within that function, increment args[1] by 1 and return.

#### Implementing Halt

Goal: Shut down the entire system. Accomplish this through calling shutdown_power_off(). Will need to include the shutdown.h library in syscall.c.
No need to worry about parent/child process dependencies because we are effectively trashing away all existing processes.

#### Implementing Exec (Overview)

In userprog/syscall.c, call process_execute(). While this function does much of the executing already, we need to implement synchronization to ensure that the parent process does not return from the exec syscall until it knows whether the child process successfully loaded its executable or not. The child will be loading its executable in the function `start_process()`.

#### Implementing Exec (Synchronization)

The thread that called exec cannot return from exec until its child has successfully loaded its executable. A mechanism for waiting is to call sema_down() at the end of process_execute() and the child will call sema_up() before asm_volatile but after loading variables in `start_process()`. 

We do sema_down() right before the call to return tid because if the thread_create call failed, we need to immediately see that and free the failed child creation, rather than waiting for a child that in reality failed to be created in the first place. However, once we confirm that the child was created successfully, our parent thread cannot exit process_execute() until the child has also finished loading its executable.

#### Implementing Wait

This syscall will call process_wait(tid_t child_t). When we created the child process, we added the child process to the child_processes list, which contains thread structs. Now, we simply need to iterate through and find the child with the corresponding tid to the child_t passed in. Return -1 if the tid is invalid (this can be checked by seeing if the thread is in the all_list) or if it is not a child of the parent (not in child_processes list). We also need to check if the child is not in a position to execute (not in THREAD_DYING or THREAD_BLOCKED state).
The act of waiting is done by calling sema_down(&child_exit) and blocking until the child calls sema_up(&child_exit). After sema_up was called from the child (note: the sema_up() call happens in process_exit), we can decrement the ref_cnt because we know the child died. However, it is possible that the parent had already exited before the child even exited, so ref_cnt could be either 1 or 0 after this decrement. If it is 0, we free the wait_status struct. (note: the ref_cnt decrement happens in process_exit to prevent decrementing ref_cnt by both wait and exit) Regardless of the value of ref_cnt, we can now return from wait by returning the child’s exit_code. 

#### Implementing Exit

We call process_exit(void). In the process, update the wait_status with its exit_code. Call sema_up(&child_exit) to signal any parent processes that are waiting on it. Now, to understand what I’m about to do next, it is important to realize that the process that is calling exit() can have 1 parent and can also have 1+ children, so we need to deal with both cases. For the parent case, decrement the ref_cnt. The wait_status struct is unused now, and if the parent is also already dead (if ref_cnt is 0), then we can also free this wait_status struct. For the children case, recall that each thread has a list of children for which it shares a wait_status struct with each. Since the child is exiting, these wait_status structs will no longer be in use, and if their ref_cnt is 0, we free the struct as well. Finally, after this lengthy process, we can terminate the thread.

- In Pintos 1 process could carry only 1 thread due to that reason, either pid or tid could be used interchangeably. However, wait(pid) call is waiting for either - 1 if the kernel is interrupted or a successful return from the child. So, an exit from the child is necessary after a call of wait(pid) function.
- process_wait() from userprog/process.c could be changed so that this syscall works correctly.
- Semaphore data structure is used in the function

### Synchronization
#### Wait/Exit Synchronization

[This refers to the synchronization aspect of Wait/Exec. See above for the struct and algorithm of Wait/Exec]

When a parent waits on a child process, we should obviously make sure the child process is currently alive. If the child is already dead, then the wait_status and the waiting itself is not necessary (wait can return). However, as the parent waits on the child, 2 things could happen:

Case A: The child can exit while the parent is still waiting. In this case, the parent can stop waiting and wait_status can be freed.

Case B: The parent exits but the child hasn’t exited yet. The child still has not exited, so we cannot free the wait_status struct yet, until the child exits as well.

To facilitate this, we add the following entries to the wait_status struct:
* struct semaphore child_exit
- On initialization of the wait_status struct, this semaphore will have the value 1. When the parent starts waiting for the child, it will call sema_down(&child_exit). This sema_down() call simulates the parent waiting until the child exits. When the child exits, it signals this fact to the parent by calling sema_up(&child_exit).
* int ref_cnt
- The purpose of this variable is to know when to free the wait_status struct. Free when ref_cnt is 0.
- ref_cnt is 2 when parent & child are alive, 1 when only 1 of the 2 are alive, and 0 when both are dead. 
- If a parent is waiting on a child, then the child exits, ref_cnt decreases to 1. However, the parent is still alive and the wait_status struct will not be freed until the parent exits as well.
- If a parent calls wait, but then exits before the child exits, ref_cnt decreases to 1. Wait_status is not freed until the child exits.
- In both cases, it is necessary to delay freeing wait_status in case the zombie process wakes up again.

#### Exec

As mentioned in the previous part, exec requires synchronization to ensure that the parent does not return from process_execute() until the child has successfully finished loading its registers. 

#### Enforcing Syscall Order

There’s a single kernel stack used for all syscalls. Use a semaphore
Make sure kernel doesn’t write to user memory.
Tradeoff is that we may slow down the processor, as if a thread wants to do a syscall, and the kernel is already processing another thread’s syscall, the thread would have to wait.

### Rationale

The alternative data structure for semaphore is lock. However, the lock is controlled by the owner and we do not require an owner in semaphore. Due to that reason, the semaphore is more useful to use for our implementation because we have a general queue of syscalls that triggers the calls instead of an owner.
The time and space complexity of practice syscall is O(1) because we are not holding any variables and increase the value by 1 and return. The time and space complexity of halt syscall si O(1). In halt syscall the operating system will be killed. The time and space complexity of exec syscall is O(1). The time and space complexity of wait syscall is O(N) and the N represents the number of children.


## Task 3: File Operation Syscalls
### Data Structures and Functions
#### Queue and Lock for Tracking Incoming Syscalls

File operation syscalls will use the same syscall queue and lock as process control syscalls (see the previous task for the relevant code and explanations). In terms of execution and queue handling, we will not differentiate between these two types of syscalls. 

#### Global Lock to Prevent File Operation Race Conditions

```
/* A global lock for ensuring that only one file operation syscall
   is handled at any given time */
struct lock global_filesys_lock;
lock_init(&global_filesys_lock);
```

Since the PintOS filesystem is not thread-safe, we will use a global lock variable to prevent race conditions. The syscall handler in userprog/syscall.c will acquire this lock before handling any kind of file operation syscall, and release the lock after completing the operation. Any pending file operation syscalls will need to wait for the lock to be released.

#### Keeping Track of File Descriptors

```
static int FD_MIN = 2; /* 0 and 1 are reserved fds */
/* to prevent overflow, the kernel will prevent more than FD_MAX fds from being created at one time */`
static int FD_MAX = 2000000000; 
struct file_desc {
    struct list_elem elem;
    int fd;
    struct file *file_ptr;
};
 
struct list file_desc_list;
```

To make some syscalls work, PintOS will need to associate each file with an integer called a file descriptor, or fd. We will keep track of each file descriptor using a linked list of file_desc objects, each of which will contain an fd ranging from FD_MIN to FD_MAX and a pointer to a struct file as defined in file.c. FD_MIN and FD_MAX are static variables meant to enforce non-usage of 0 and 1 (since they are reserved) and to prevent integer overflow. The global `filesys_lock` will also serve as a lock for accessing `file_desc_list`.

### Algorithms
#### General Approach

We will implement file operation syscalls using the functions from the filesystem libraries in pintos/src/filesys. In userprog/syscall.h, we will extend `syscall_handler()` to handle file operation syscalls in much the same way that we extended it to cover process control syscalls. Namely, we will add a switch statement to `syscall_handler()` that executes the correct syscall based on what syscall number is passed in.

#### Implementing create

The create syscall will call `filesys_create(const char *name, off_t initial_size)` found in filesys.c:45, using the two arguments `const char *file` and `unsigned initial_size` from the syscall. `filesys_create` will return true or false depending on whether the file creation was successful, which is exactly what the syscall itself will return.

#### Implementing remove 	 	

The remove syscall will make a call the `filesys_remove(const char *name)`, found in filesys.c:84, using the `const char *file` argument to the syscall. `filesys_remove` will return true or false depending on whether the file removal was successful, which is exactly what the syscall itself will return.

#### Implementing open

The open syscall will call `filesys_open(const char *name)` in filesys.c, which returns the pointer to a struct file as defined in file.c. Instead of returning the struct file pointer directly, the syscall will insert a new node into the `file_desc_list`, the global Pintos list associating file descriptors will struct file pointers. If filesys_open fails and returns a null pointer, the syscall will return -1 without altering the `file_desc_list`.

#### Managing File Descriptors

There are a number of edge cases to handle when it comes to file descriptors. To enforce the non-usage of 0 and 1 for files, we ensure that all file descriptors are greater than `FD_MIN=2`. To make sure that file descriptor integers do not overflow, we check that all descriptors are less than or equal to `FD_MAX`, preventing the creation of any further file descriptors if this limit is exceeded. 

Also, we want to make sure to reclaim file descriptor numbers after they are removed from the `file_desc_list`. Our algorithm for doing this is to always assign descriptor numbers from low to high, inserting them in ascending order into the `file_desc_list`. When a file is opened, we find the lowest available valid file descriptor by traversing the `file_desc_list`. When a file is closed, we remove its node from the `file_desc` list. See Time and Space Complexity below for a detailed description of the efficiency of this algorithm.

#### Implementing filesize

The filesize syscall will first traverse the `file_desc_list` to identify the struct file pointer associated with the file descriptor passed in. See Time and Space Complexity for the efficiency of this algorithm. Once the struct file pointer is found, the syscall will use it to access and return the output of file_length() in file.c. Should Pintos fail to find the struct file pointer, the syscall will return -1 to indicate failure.
Implementing read and write
Like filesize, read will first find the appropriate struct file pointer in `file_desc_list`, returning -1 if this cannot be found. Then, it will call `file_read` in file.c and return its return value. Should 0 be passed in as the file descriptor, filesize will instead call `input_getc()` to access input from the keyboard.

Syscall write will go through the same process, calling `file_write` instead of `file_read`. The write may fail even with the right struct file pointer (for example, if the target file is a running executable), in which case, the syscall will return -1.  Should 1 be passed in as the file descriptor, write will call `putbuf()` to print to the console, breaking up buffers larger than 64 bytes to prevent race conditions on the console.

#### Denying Writes to Running Executables

In order to ensure that no file operation writes to a running executable, we will alter `start_process()` and process_wait() such that the former opens an executable before loading it and the latter closes said executable upon the termination of a process, whether through `process_exit()` or by an exception. The start_process function will call `deny_write()` on the executable file, ensuring that no file operation can write to it while it is running. This deny_write automatically expires when the file closes. `start_process()` and process_wait() will share the file descriptor through the same data structure used to implement wait and exit (see Task 2: Data Structures and Functions for details).

#### Implementing seek

Seek will translate its file descriptor to a struct file pointer, returning without doing anything if this operation fails. On a success, it calls `file_seek` in file.c.

#### Implementing tell

Tell will translate its file descriptor much like seek, returning -1 on a failure. On a success, it calls and returns the output of `file_tell` in file.c.

#### Implementing close

The close syscall will translate its file descriptor, returning without action on a failure. On a success, it first removes the file descriptor’s node from the `file_desc_list` to prevent other processes from accessing it. Then, it calls `file_close` in file.c.

#### Edge Case: Handling Invalid Arguments

It is possible that `intr_frame` *f either is an invalid pointer or does not point to a properly set up `intr_frame`. We will test for this case by validating `f`, and if applicable also `f[0]`, `f[1]`, `f[2]`, and so on. For file operation syscalls, this is done in much the same way as process control syscalls. We will use the same procedure to verify struct file pointers returned by file library functions. See Task 2: Algorithms for details.

If an invalid syscall number is passed into the syscall handler, we will handle it by terminating the caller user process. 

#### Time and Space Complexity

The `file_desc_list` takes of O(N) space, where N is the number of open files among all processes at a given time. Inserting a single node into the `file_desc_list`, which happens every time a file is opened, takes O(N) time since we could potentially traverse the entire list to find the lowest available valid file descriptor. Removing a single node from `file_desc_list`, which happens when a file is closed, takes O(N) time to find the relevant list_elem and O(1) time to actually remove the node. Each time the write or read syscall is called, Pintos needs to traverse the list to find the struct file pointer corresponding to the file descriptor passed in, which also takes O(N) time. 

### Synchronization 
#### Race Conditions Between Syscalls

The general syscall queue and lock, described at length in Task 2: Data Structures and Functions, prevents race conditions between file operation syscalls and process control syscalls. The global file operation lock in Task 3: Data Structures will prevent race conditions between different file operation syscalls. To prevent deadlocks, we make the former lock a prerequisite for obtaining the latter lock. 

#### Race Conditions Between exec and write

Some functions in process.c make calls to filesystem library functions without going through a syscall. When possible, we will change these functions to make use of syscalls instead of said library functions. If this is not possible, we will ensure that the process.c function acquires the global file system lock before calling any library functions. This way, there will be no race conditions between file operation syscalls and process functions. Incidentally, this approach also prevents race conditions between `process_start()` and `write()`, resolving an edge case that might otherwise result in a successful write to a running executable.

### Rationale
#### Trade-offs of the File Descriptor Data Structure

When building the data structure to store file descriptors, we had to decide whether to use a linked list mapping file descriptors to pointers or an array of struct file pointers. The latter would have made it easier to implement file descriptor assignment, since we could have avoided implementing comparison operators for file descriptors. However, we went with the linked list because it offered more flexibility in terms of memory usage and varying numbers of open files. 

Another option we considered was casting pointers directly to file descriptors, allowing us to forgo a data structure altogether. Although this route would have been arguably easier to implement and more performant, we decided against it after considering potential difficulties and assumptions that could arise from casting unsigned pointers to ints.

#### Trade-offs of the Global Lock

Before deciding on a global lock, we also considered using semaphores. Since this particular application does not require the additional flexibility of values offered by a semaphore, we decided it would be simpler just to use a lock. Also, we decided that having a separate locking mechanism for different files would be too complex to be worthwhile for a non-thread-safe filesystem.

#### Shortcomings and Expanding Functionality

Our current implementation of file descriptor translation runs in O(N) time, which may not be optimal. With a different data structure, we might be able to achieve O(lgN) with binary search or even O(1) with hashing. For now, we have decided that the additional complexity and memory usage of these solutions outweigh their benefits. 

The approach we chose for tracking open executables does mean that file operation syscalls may have to share a lock with non-syscall functions (e.g. to set deny_write on executables before running them). Nonetheless, with only a single lock shared between these processes, we have ensured that deadlock cannot occur between them. For the moment, we have decided that this approach is the best one for preventing race conditions.

Our use of a global lock limits the throughput of the filesystem, which is okay for now given that the filesystem is not thread-safe. However, this lock will have to be replaced with a different form of synchronization if we decide to expand filesystem functionality to include concurrency. Such a system might be difficult to design, but one advantage of the global lock system is that it is relatively simple to dismantle and replace.

## Additional Questions

1. Take a look at the Project 1 test suite in pintos/src/tests/userprog. Some of the test cases will intentionally provide invalid pointers as syscall arguments, in order to test whether your implementation safely handles the reading and writing of user process memory. Please identify a test case that uses an invalid stack pointer (%esp) when making a syscall. Provide the name of the test and explain how the test works. (Your explanation should be very specific: use line numbers and the actual names of variables when explaining the test case.)

* pintos/src/tests/userprog/sc-bad-sp.c
This test calls asm volatile on line 18 to run assembly. This assembly code moves a bad address (approximately 64MB below the code segment, according to the comment) into `%esp` and then calls int $0x30 to start a syscall. This results in a bad address being passed into the syscall handler in userprog/syscall.c:16 as `f->esp`, where `f` is the pointer to the struct `intr_frame` used to copy data from the user stack. If the syscall_handler causes a page fault by trying to dereference `f->esp`, it fails the test.

2. Please identify a test case that uses a valid stack pointer when making a syscall, but the stack pointer is too close to a page boundary, so some of the syscall arguments are located in invalid memory. (Your implementation should kill the user process in this case.) Provide the name of the test and explain how the test works. (Your explanation should be very specific: use line numbers and the actual names of variables when explaining the test case.)

* pintos/src/tests/userprog/sc-bad-arg.c
This test calls asm volatile on line 14 to run assembly. This assembly code moves an address that is the top of the stack into `%esp` and then calls int $0x30 to start a syscall. When the syscall handler tries to access the arguments above the %esp, which is at the top of the user address space, like on line 31 of userprog/syscall.c, it will go into kernel memory and cause a page fault. The syscall handler should have a check to make sure it does not try to access invalid memory.

3. Identify one part of the project requirements which is not fully tested by the existing test suite. Explain what kind of test needs to be added to the test suite, in order to provide coverage for that part of the project. (There are multiple good answers for this question.)

* The test suite does not have enough coverage for combining multiple types of syscalls, including combining file syscalls and syscalls from Task 2. We can add tests which include different combinations of both in different orders.
