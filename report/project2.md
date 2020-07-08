
Final Report for Project 2: Scheduling
=========================================
### Task 1
*  It became apparent that waking up threads by invoking an external function really was not the way to go. After insightful consultation with Annie Ro, a simple `thread_unblock` call in `timer_interrupt` was all that was needed.
	 -  Instead of having a newly created external function “keep looping across threads until it can wake up a thread”, we simply needed to make use of the fact that `timer_interrupt` would be invoked externally every tick, so we could have `timer_interrupt` do the job of waking up threads.


### Task 2
* Besides the `donor` and `awaited` data structures, we also added a list of `locks_held` to the `struct thread` data structure.**
	* In `lock_release`, we iterated through the `locks_held`. Of these locks’ owners (which are threads), we set the current thread’s donor to the thread with the highest effective priority.
-   In `sema_up` and `cond_signal`, we chose the next thread from the list of waiting threads based on priority.

-   We checked if the thread needed to be yielded to the scheduler each time the priority changed.

	-   In `sema_up`, we checked to see if the new thread had a higher priority than the current thread. If it did, we would yield to the new thread; otherwise, we would finish running the current thread.
    
	-   In `lock_release`, we checked to see if the current thread was still the thread with the maximum priority. If it is no longer the max, we yielded.

		-   We calculated the thread with the maximum priority through the helper function `max_priority_thread` in thread.c, where it iterated through the list of all ready threads and found the one with the highest effective priority.
    

 ## Who Did What?

A reflection on the project – what exactly did each member do? What went well, and what could be improved?

-   Michael and Crystal took down the goliath that of which was Task 2 while Daniel and Onur emerged from the pile of Task 1 bugs in triumph. Daniel and Onur also collaborated in finishing the scheduling lab.


## Scheduling Lab
1a.

0: Arrival of Task 12 (ready queue length = 1)<br>
0: Run Task 12 for duration 2 (ready queue length = 0)<br>
1: Arrival of Task 13 (ready queue length = 1)<br>
2: Arrival of Task 14 (ready queue length = 2)<br>
2: IO wait for Task 12 for duration 1<br>
2: Run Task 14 for duration 1 (ready queue length = 1)<br>
3: Arrival of Task 15 (ready queue length = 2)<br>
3: Wakeup of Task 12 (ready queue length = 3)<br>
3: IO wait for Task 14 for duration 2<br>
3: Run Task 12 for duration 2 (ready queue length = 2)<br>
5: Wakeup of Task 14 (ready queue length = 3)<br>
5: Run Task 14 for duration 1 (ready queue length = 2)<br>
6: Run Task 15 for duration 2 (ready queue length = 1)<br>
8: Run Task 15 for duration 1 (ready queue length = 1)<br>
9: Run Task 13 for duration 2 (ready queue length = 0)<br>
11: Run Task 13 for duration 2 (ready queue length = 0)<br>
13: Run Task 13 for duration 2 (ready queue length = 0)<br>
15: Run Task 13 for duration 1 (ready queue length = 0)<br>
16: Stop<br>

1b.

0: Arrival of Task 12 (ready queue length = 1)<br>
0: Run Task 12 for duration 2 (ready queue length = 0)<br>
1: Arrival of Task 13 (ready queue length = 1)<br>
2: Arrival of Task 14 (ready queue length = 2)<br>
2: IO wait for Task 12 for duration 1<br>
2: Run Task 13 for duration 2 (ready queue length = 1)<br>
3: Arrival of Task 15 (ready queue length = 2)<br>
3: Wakeup of Task 12 (ready queue length = 3)<br>
4: Run Task 14 for duration 1 (ready queue length = 3)<br>
5: IO wait for Task 14 for duration 2<br>
5: Run Task 15 for duration 2 (ready queue length = 2)<br>
7: Wakeup of Task 14 (ready queue length = 3)<br>
7: Run Task 12 for duration 2 (ready queue length = 3)<br>
9: Run Task 14 for duration 1 (ready queue length = 2)<br>
10: Run Task 13 for duration 4 (ready queue length = 1)<br>
14: Run Task 15 for duration 1 (ready queue length = 1)<br>
15: Run Task 13 for duration 1 (ready queue length = 0)<br>
16: Stop<br>


2a.

Open System. This is because even though the arrival time of task k depends on the arrival time of task k-1, it does not depend on the completion of task k-1. In essence, only the order of the arrival times are regulated, but we can keep having bursts arriving without previous bursts having completed.

2b.

The expected value (mean) of an exponential distribution is 1/<img src="https://latex.codecogs.com/gif.latex?\lambda" /> , so solving for 1/<img src="https://latex.codecogs.com/gif.latex?\lambda" /> =<img src="https://latex.codecogs.com/gif.latex?M" /> , we get =1/<img src="https://latex.codecogs.com/gif.latex?M" />.

2c.

For 50% CPU utilization, you would want the average time between arrivals to be 2<img src="https://latex.codecogs.com/gif.latex?M" />. This means <img src="https://latex.codecogs.com/gif.latex?\lambda" /> =1/(2<img src="https://latex.codecogs.com/gif.latex?M" />).

2d.

![](https://lh4.googleusercontent.com/j39EHnpMu9SEjyfIVFL7Bu2ZmXIebRFbgxJnQTKhkbXH7nw168AaVv3YWRk36-IWR9Fygl4dyuMerJVkm80r1GQZAZW2bHQ67fXYHFs9CqSvzVbkxtrdd7h8otHr_pxFTnlMLFaX)

This graph above is generated from using M = 6.2, q = 2, and round robin scheduling.

Lambda is proportional to the arrival rate. As we increase the arrival rate, our utilization also increases. This makes sense because if incoming requests come more frequently, then the CPU has less time to take a break.

This also confirms our answer from 2c., since 1/2<img src="https://latex.codecogs.com/gif.latex?M" />=1/(2*6.2)=0.08, in other words, at <img src="https://latex.codecogs.com/gif.latex?\lambda" />=0.08, the utilization is indeed roughly 50%.

2e.

![](https://lh5.googleusercontent.com/lSQUiG70DBfoijjaM2k7IXGXKsddsxi9kBrC64h0QRzkxXZ7wDZWZfeskLTB9884w-RrFYODWjhl27ABxvx-Nv3zQIvNfiQOD5P-JI2xV10og55KtLV62UPypuhjFqQ_lXQL43GG)

As we increase the arrival rate, the response time also increases. This makes sense because if we bombard the CPU with a ton of new tasks (high arrival rate), it will have to switch between each task more, causing each task to take longer to finish.

  

2f.

SRTF:
As shown in the graphs below, if we change from round robin to SRTF (with everything else the same), our average response time curve flattens significantly. This makes sense since we prioritize shorter tasks first, so we are able to complete these tasks quicker on average. However, the 95th percentile remains quite slow, as longer tasks still will end up taking a long time to complete.

![](https://lh6.googleusercontent.com/FVwjRBgRPt7bT6ZV9Ve8JYfXe1P3_TCczXIObFVhLM6cqR6Fe05W91Y0lc0EBLt8uTXAIcvtiCMbAOiOPiLoF1Id5ihakAOrEUwIfa-8r3mvjKS5E9wHBzotVAbv4E_aXJyeQ8nH)

  

MLFQ:

Using q1 = 2 and q2 = 5.

![](https://lh4.googleusercontent.com/GLaMbXG134hXUeDQBFv9A8q5yG4i1B0ZnTLl9G2LPgJqDLYzVWfMynb9YnDl-0ptwrJc04gTocjKgoi-mOOhBr81pMRBmpAcw665KtWQVAyeglIPPapR5CuDQ_Sh7TiwIfC9Uv_h)

MLFQ seems to slightly improve the arrival rate but the curve is mostly the same as round robin. This is because our MLFQ implementation uses round robin for both queues, but it still demotes longer tasks through 2 queues so we don't get caught up on longer tasks.

  

Crashing the CPU:

If we kept the old round robin but made M huge (M = 25), then our system breaks. Since each burst takes so long to complete, our CPU utilization becomes effectively 100% for higher arrival rates, making the response time go into mayhem.

![](https://lh4.googleusercontent.com/y2wdHqDUJp4jFANpRippBa8Egx2TxeMi9WGz4TUExPp7o3YfTke2qXNnkwtcW8zp_UDfzdQSgd3dW6SKlsWPYuoVC_2pukPe2zggP4jjOorxgfHXrc2P4zt3AGH1SScOOMyvIZLy)![](https://lh3.googleusercontent.com/VPthBA449ZQmyKLtmCgEqPWQfwvdH2QZiU3Leo2Vlh3s8HISwPHAwOsMbcTnV8N6zoRHWz78qRbklh7k6Xt7iOkMUfPL9V-UHOOaWDwsc8EhNU8ap16HXPBqPdwpTuUp80o0AbQu)

  

2g.

As shown in the Arrival Rate vs Response Time curves, if we crank up the throughput too high, the CPU spends too much time juggling between tasks, and it takes longer to complete any task. This is called thrashing. While it's not modeled in this simulation, the overhead to context switch would also increase response time for each task.



3a.

Since S and T are periodic tasks, Si-1 should be completed before Si placed into the queue or Ti-1 should be completed before Ti placed in the queue. Due to that reason, at most 2 tasks can be placed at a time in the FCFS queue.

  

3b.

Both S and T have the same burst length distribution due to that reason they will have equal possibility for being greater or less than each other. This will yield that Pr[S1 < T1] = 0.5.

3c.

The bursts lengths are given as independent and identically distributed random variables. Due to that reason, we could assume that the expected value of each burst will be the same and the mean will be equal to the expected value. On the other hand, the variance will be equal to the variance of a burst divided by the total number of bursts which is m. The normal distribution of CPUTime(S) will be N(E[Si], Var(Si)/m)

  

3d.

The burst length distributions are the same for both S and T. So, the mean of CPUTime(T) will be equal to E[Si] but since CPUTime(S) is multiplied with n the mean will become n <img src="https://latex.codecogs.com/gif.latex?*" /> E[Si]. For the variance, the variance of CPUTime(T) equals to Var(Si)/m and the variance of n <img src="https://latex.codecogs.com/gif.latex?*" /> CPUTime(S) will be n <img src="https://latex.codecogs.com/gif.latex?*" /> n <img src="https://latex.codecogs.com/gif.latex?*" /> Var(S_i)/m.

  

![](https://lh5.googleusercontent.com/4p1e8igioJgffzEiPOG0LTJG76rqlh7ByVM8j2qp7iiEq0NA8P_a0xETNO5XXCgTnqr5jbWqDZARA7yZBb1Q3NG52I6Phv04EDwe8OOE4uwQ9b1S2hjikAf6494VI85PbXcCcxFa)

  

3e.

Assume Var(Si) = E[Si]*E[Si]. The probability equation will become

![](https://lh6.googleusercontent.com/9jBAdxI4eIdh5NCjM6WFnFSyW3DiZFdJHDgvh4NM5-_pG_gWbs1lggwUUayNKm37AnNiTZUdV5Cj3yNkUuzou7vzBCVWl7FPDvPo89y39SswK3rehpjah460AvgYZhQOtPzYYU1p)

For m = 100 case, to find the probability that CPUTime(S) is 10% greater than CPUTime(T), n should be equal to 1.1, to find the probability that CPUTime(T) is 10% greater than CPUTime(S) n should be equal to 1/1.1. So, for m = 100 case the probability will become

![](https://lh5.googleusercontent.com/cfJ9Y4R01KLX5A7G9IDs75hbUiPWU7r-A29M8icCEozLFd7as0Foh2V2-g3y9omo-dz8VpeEU1lFYtvjBzZVAvlZHWdl732dUZfz23SmNxj92FImROI1NQG582x1P9LGH7vFnm_X)

For m = 10000 case, the probabilities will become

![](https://lh5.googleusercontent.com/zBHsfEryJ5kT_Ilcel-fLAz5jT8TfbzTvjfAYjmEL-oMOg5zWIyLAeGez0Q8h95Q8hqnFXiWAUljCWxh17PcocqXa4KVxA3u4iBGnIAruhvLWW3t4zEmfVFeZre0mUFaCywwL2XU)

According to the results above, Samuel’s reasoning is false.
