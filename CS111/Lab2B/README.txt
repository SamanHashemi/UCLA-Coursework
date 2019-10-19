NAME: Saman Hashemipour
EMAIL: hash.saman@gmail.com
ID: 904903562

Files:

SortedList.h: Given file with the require implementation of the corresponding .c file

SortedList.c: The SortedList file with the implementation of a doubly linked sorted list.

lab2_list.c: The bulk of the program for protected operations on a linked list from above

lab2_list.csv: The information from output of the file above.

lab2_list.gp: The script that generates all the pictures below

lab2_[1-5].png: Graphs of relevant data from above

profile.out: The execution profile of the threads running with the info about how much of a CPU hog each one was.

Question 2.3.1:

Most of the time is spent in operations in the 1-thread program runs. For the 2 thread runs with Spin locking time seems to be split, but with mutex most of the time is spent performing operations.

This is the most expensive part of the code because the critical sections are where the locks take place and without them we would have undeterminanistic code but with them we take up a lot of time. Also, the operations taking place in these sections are, by themselves, expensive.

In high thread spin locking programs most of the time will go to spinning.

But most of the time is spent actually doing the operations in mutex locked programs.

Question 2.3.2:

The most CPU time is consumed by the spin locks. Specifically, the line of code that acquires the spin lock takes up the most time. A majority of the time is spent on this line trying to run the loop until we are ready to move on.

This takes up a lot of time because instead of actually performing the the operations, the spin lock takes up most of the time just spinning. The threads just waste the time slice that it is allotted by spinning and with large numbers of threads we increase the amount of spinning and thus the amount of time that is wasted.



Question 2.3.3:

The average time spent waiting by threads goes up dramatically because there are more threads that are just waiting around to acquire the lock. The lower the number of threads, the less threads are trying to acquire the lock.

The completion time increases because there are a number of threads that need to complete performing operations before the program is done running.

If the number of threads is so high that most of the time is spent waiting around rather than actually performing the operations, the amount of time spent waiting therefore, could exceed the time it takes to complete the operations.


Question 2.3.4:

As we increase the number of lists, we also increase the overall throughput. This is because as we deal with each sublist we the amount of time spent on each sublist also goes down. The overall time spent on each list also goes down because the length of each list is decreased.

The throughput should not forever increase. The throughput increases at a slower rate as the number of lists goes up. This is because as we increase competition for the locks which will eventually become competing for the locks more than we are performing operations and throughput will not increase as quickly.

This is not necessarily true. We saw from the answer above that as we increase the number of threads the completion for locking goes up and thus slows the whole program down. We see that lock competition is high for high thread programs but the N-way partitioned list would spend more time locking and thus not perform the same as the threaded approach.









