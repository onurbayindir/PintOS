Design Document for Project 2: Threads
======================================

## Group Members

* Crystal Jin <crystalj@berkeley.edu>
* Daniel Sun <daniel.s@berkeley.edu>
* Michael Wang <mywang@berkeley.edu>
* Onur Bayindir <onurbayindir@berkeley.edu>

## Task 1: Efficient Alarm Clock
### It’s time to DD-D-D-DDDDDD-Data Structures and Functions
```
struct waiting_thread {
    tid_t thread;
    int64_t start;
    int64_t ticks;
    struct lock *awaited;
};
```

Linked list containing `struct waiting_thread` containing the following:

* Thread struct of current thread
* Start (this variable represents the initial time tick)
* Ticks (how long we have to wait)
* list_elem

### Algorithms
In `timer_sleep()`, after the call to `int64_t start = timer_ticks ();`, we add the current thread, start, and ticks variables to our global linked list. 

* We will have a function that loops through the linked list and checks if `timer_elapsed (start) >= ticks` for each of the linked list elements.  

After adding these variables to our global linked list, the next time the above mentioned function loops through the linked list, it will also check our newly added thread sleep request. In the meantime, we block the current thread.  
When the function that loops through the linked list sees that `timer_elapsed (start) >= ticks`, it will unblock the thread, and yield it to the RUNNING queue. On top of that, if the input is equals or less than zero then the thread will be yielded to the RUNNING queue immediately after the function call without going into the while loop.  

### Synchronization
Disable the interrupts before the `thread_block()` to prevent interruption of the current thread during sleeping. Enable the interrupts just after the `thread_unblock()`. Since the Linked List implementation is not thread safe, we planned to lock before each operation that involves the Linked List to prevent modification from other threads.

### Ra-ra-rationale, Lover of the Russian Queen
The main goal is to ensure that we do not busy wait. We prevent this by removing the while loop entirely, and replacing it with a thread block/unblock mechanism. We also created an external function that does the work of waking up threads when necessary.

## Task 2: Getting Your Priorities Straight
### Data Structures and Functions
```
struct thread {
    // (Pre-existing data members)
    struct thread *donor; //thread that donated
    struct lock *awaited;
};
```
We will add two data members to struct thread: 
* `struct thread *donor` will keep track of which thread donated the priority, so that we can judge when to undo the priority donation.
* `struct lock *awaited` is the lock that this thread is waiting on

### Algorithmae
#### Choosing the next thread to run
When a running thread is preempted, the scheduler first identifies the highest priority level of any thread in the `ready_list` and stores it as `hi_priority`.

* `next_thread_to_run()` should then iterate through the `ready_list` and return the first thread in that list with effective priority `hi_priority`.
* If the `ready_list` is empty, `next_thread_to_run()` shall return the idle thread.
* Since `ready_list` is itself built up in FCFS order, this procedure guarantees a round-robin execution order among threads of the same priority.

Once the appropriate thread is selected, the scheduler removes it from the `ready_list` and runs it for `TIME_SLICE`.

#### Acquiring a Lock
In the function `lock_acquire`, before the `sema_down()`, we check to see if the `lock->holder` is null. If it is, that means that no thread is currently holding the lock, so this thread will acquire the lock next. That means we do not need to donate any priority.  
If it is not, that means another thread is currently holding the lock. 

* We set the `struct lock *awaited` to the lock if `lock->holder == NULL`. Once we wake up , we set `awaited` to `NULL` again to indicate that we are no longer waiting for a lock.
* We then check to see if the current thread’s priority is lower than the lock holder. If it is, we donate priority by setting the lock holder’s `struct thread *donor` to the current thread, and the `thread_set_priority()` function will calculate the effective priority.
* We must then donate to the chain of locks being waited upon, by checking if the lock holder of `struct lock *awaited` (if any) has an effective priority that is lower than the current thread, and if so, we set the lock holder’s `struct thread *donor` to the current thread. We then perform the same operation on the lock holder. We stop the donation chain if the next thread has a higher priority or there is no next thread.
* For each donation, we are donating effective priority in the case that a thread is awaiting a lock but also holding another lock that other higher priority threads are waiting on.

We disable interrupts during this entire check to avoid being preempted, and reenable interrupts after we change the lock holder to the current thread.

#### Releasing a Lock
Upon releasing a lock, a thread first needs to check if it has received a priority donation.

* If `struct thread *donor` is not `NULL`, then check if `donor->awaited` is the same lock as the one that is being released.
* If so, the current thread no longer needs its priority donation, so it sets `donor` to `NULL`. But if the `donor` is not waiting for the lock presently being released, then the current thread should keep its priority donation and keep `donor` unchanged.

We disable interrupts during this check to avoid being preempted, and reenable interrupts after the call to `sema_up`.

#### Computing the effective priority
This is done in the `thread_set_priority()` function. We store effective priority in the form of a `struct thread *donor` data member, as shown above. 

* If `struct thread *donor` has value `NULL`, then our current thread has not received a priority donation. Return its `int priority` member.
* If `struct thread *donor` is not `NULL`, then return the priority of the donor thread, `donor->priority`.

#### Priority scheduling for semaphores and locks 
The current implementation of `sema_up` picks the first thread on the wait queue in a FIFO pattern. We will change this procedure to instead iterate through all the threads in the wait queue and choose the one with the highest effective priority. We will use FIFO to break ties, since our use of round-robin within each priority level minimizes the impact of any tie-breaking method we choose.  
Since a lock uses a semaphore internally to keep track of waiting threads, changing the thread selection procedure in `sema_up` will effectively implement priority scheduling for both semaphores and locks.

#### Priority scheduling for condition variables 
In `cond_signal`, the present implementation wakes up the first waiter in the `cond_waiters` list, regardless of its priority. We will alter this procedure to iterate through the list, waking up the first element that has the highest priority among those in the list. 

#### Changing thread’s priority
For donations, we set the `struct thread *donor` to the thread that donated its priority, and we calculate the effective priority with the `thread_get_priority()` function. For changing the actual priority of a thread, we call the `thread_set_priority()` function.

### Synchronization
#### Releasing and Acquiring Locks
We need to disable interrupts in `lock_acquire` and `lock_release` briefly if we need to perform priority donation, so that the thread does not get preempted after we have altered priorities but have not yet acquired/released the lock. If the thread goes to sleep, interrupts are automatically reenabled.

#### Concurrently Accessing and Modifying Struct Threads
We prevent a thread’s `donor` member from being modified by disabling interrupts during the process of priority donation. Thus, no thread can be preempted while it is modifying the priorities of other threads. Since only thread priorities are only accessed by the scheduler, locks, semaphores, and condition variables, disabling interrupts is enough to ensure that no process tries to access a thread’s priority while it is being changed. The same applies to the `awaited` lock pointer in each thread, since this is accessed only during priority donation 

#### Concurrently Accessing and Freeing Struct Threads
Each thread has access to a `donor` thread pointer. In the event that a `donor` is freed before the thread holding it releases its lock, there will be a page fault. However, this will not happen because the donor thread is waiting on the lock that the current thread is holding. When the current thread releases this lock, waking up the `donor`, it will set `donor` to `NULL`, meaning that it will no longer try to access `donor`.

#### Edge Case: Recursive Priority Donation
Suppose Thread A (p=3) waits on thread B (p=2), which waits on thread C (p=2). Then thread A needs to donate to B and C. But when C releases the lock, we need to account for the possibility that the lock will go to A before B since they are both at effective priority 3. The danger here is that if A exits before B, then there will be a page fault when B tries to access its `donor`, which points to A. We avoid this scenario by ensuring that locks are always distributed in according to FCFS within the same priority. For example, in this scenario, since B started waiting for the lock before A, it is guaranteed to receive the lock before A, avoiding any chance of a page fault.

### Ra-ra-rationale, Russia’s Greatest Love Machine
* Instead of tracking the effective priority as its own integer data member, we decided to simply track which thread donated the priority. Since donated priority is equal to the priority of the donating thread, we have access to this information without storing it explicitly.
* We did not add anything to `lock_try_acquire()` because that will not work with priority scheduling, since if a high priority thread tries to acquire a lock, it will busy wait, and lower priority threads will never get selected to run.
* We considered using an array to store priorities, but we decided on the easier implementation of just iterating through all the waiting threads each time we schedule a new thread to simplify priority donation.

## Additional Questions
1. In thread.c, the function `schedule()` calls `switch_threads(cur, next)`, which is an assembly function that makes thread `cur` that is currently RUNNING be put on the READY queue, and puts `next` in RUNNING state. The program counter, stack pointer, and registers are pushed to the kernel stack in `switch_threads`, and the function changes its `thread_stack_ofs` (which is the pointer to the corresponding kernel stack) to the new thread’s kernel stack, where it can find the new thread’s saved program counter, stack pointer, and registers. The new thread is automatically allowed to “return” from `switch_threads`.  
2. The function `thread_exit` calls `schedule` to schedule the next thread. At the end of `schedule`, after calling `switch_threads`, it calls `thread_schedule_tail`, where it frees the page containing the dying old thread’s stack and TCB by calling `palloc_free_page`. We do not want to call `palloc_free_page` directly in thread_exit, because we want to switch threads before destroying the old thread’s memory. The comment in `thread_schedule_tail` explains, “This must happen late so that thread_exit() doesn't pull out the rug under itself.”  
3. `timer_interrupt` is called through an external interrupt from `intr_handler` in `interrupt.c`. Since it is external, it runs in the kernel stack of the thread that is being interrupted.  
4. We can design a test where there are two threads, B and C, both waiting on a lock X, currently held by A. B has a lower base priority but a higher effective priority than C, as B is holding on to a lock Y that another thread D is waiting on. D has higher priority than B, so it has donated its priority to B. Each thread will print out its name, so thread A prints out “A,” thread B prints out “B,” etc. When A releases X, the scheduler will choose from B and C to run next. We expect B to be chosen, as it has the higher effective priority, but the faulty implementation will choose C, as it has the higher base priority. Therefore, expected output is “B” but actual output is “C.”
