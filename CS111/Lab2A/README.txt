NAME: Saman Hashemipour
EMAIL: hash.saman@gmail.com
ID: 904903562


The files included are

lab2_add.c: a program that runs using shared variable add function
SortedList.h: a provided header to help develop the .c file
SortedList.c: a sorted doubly linked list implementation
lab2_list.c: a program that provides the output seen below (csv and png)
lab2_add.csv & lab2_list.csv: all the results from the tests in the two parts of this lab
add.sh & list.sh: all of the tests for both parts of the lab (add and list respectively)
Makefile: to create the program using specific functions
README: a file containing the answers to all the questions below and the descriptions listed above.

Question 2.1.1 - causing conflicts:

Why does it take many iterations before errors are seen?

This is because as we increase the number of iterations we increase the number of all the operations that we have and thus increase the possibility of the multiple threads trying to get the same critical section. This will then obviously create more errors.

Why does a significantly smaller number of iterations so seldom fail?

This is similar to the answer above. When we have less iterations we will be able to complete each thread before we start the next one. Thus, all the critical sections in the code will be able to become completed and the number of errors will be much less.


Question 2.1.2 - cost of yielding:

Why are the --yield runs so much slower?
Where is the additional time going?

--yield is so much slower because when we have a thread call the yield we wee that we have to context switch picking another thread and all the time that we spend context switching adds up and makes yield much slower.



Is it possible to get valid per-operation timings if we are using the --yield option?
If so, explain how. If not, explain why not.

We can't get valid per-operation timings because we don't know what context switches are so we then don't know what threads are running. This would be impossible so we don't know how to get per-operation timing when using --yield. 


Question 2.1.3 - measurement errors:

Why does the average cost per operation drop with increasing iterations?

We know that creating threads can be expensive and that the average cost per operation must include the cost of the thread creation itself. Therefore, as we increase the number of iterations we can say that the cost of thread creation is minimal and cost per operation goes down with it. 

If the cost per iteration is a function of the number of iterations, how do we know how many iterations to run (or what the "correct" cost is)?

We just need to run the program with a number of iterations until the cost per iteration becomes flat and we can then say that the cost of thread creation is nothing and we have found our "correct" cost.

Question 2.1.4 - costs of serialization:

Why do all of the options perform similarly for low numbers of threads?

When we have few threads we can assume that less of the threads are going to be accessing the critical sections of one another so we use less locks and thus save time per operation. This means that the bulk of the cost will come from the operations and we can say that these options will perform similarly. 


Why do the three protected operations slow down as the number of threads rises?

Since I mentioned earlier as we increase the number of threads the number of threads trying to access different critical sections around the code increases, invoking more locks and thus slow down the operations.

Question 2.2.1 - scalability of Mutex:

Compare the variation in time per mutex-protected operation vs the number of threads in Part-1 (adds) and Part-2 (sorted lists).

Comment on the general shapes of the curves, and explain why they have this shape.

Comment on the relative rates of increase and differences in the shapes of the curves, and offer an explanation for these differences.


Since the curves are both on a positive slope and the time per operation increases as the thread count increases, we notice that the shape of the graph is because the number of threads increases and we have a higher probability of getting locked trying to access critical sections thus slowing everything down.

Part-2 has a much greater increase in time per operation than part-1 when the number of threads is increased. This is due to the operations in part-2 simply being much more costly than the operations in part-1.

Question 2.2.2 - scalability of spin locks

Compare the variation in time per protected operation vs the number of threads for list operations protected by Mutex vs Spin locks. 
Comment on the general shapes of the curves, and explain why they have this shape.
Comment on the relative rates of increase and differences in the shapes of the curves, and offer an explanation for these differences.

This is similar to the answer in 2.2.1, but to reiterate, the increase in time as the number of threads increases is primarily due to the fact that multiple threads will try to access multiple sections of code that will eventually lead to more spin locking which in-turn takes more time. 

When we have a lower number of threads the we have similar time but as we increase the number of threads we see that more and more spin locks are being used and thus slows down the entire process because the spin lock doesn't give up the CPU until the time slice has expired so we can see that if we choose to use spin locks for high thread processes we will likely have very poor performance.


