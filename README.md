This program simulates CPU process scheduling using a Binomial Heap data structure. 
It dynamically calculates process priorities and runs simulations to find the optimal time quantum that minimizes the average waiting time.

How to Compile on Linux?
gcc main.c -o scheduler -lm -ansi -pedantic 
followed by:
./scheduler
(lm is necessary for math functions)

Example input: (process id, workload, arrival time)
P1 10 0
P2 5 2
P3 8 3

As Output: The program prints the heap state at each time step and a final summary of waiting times + the optimal quantum(s) found.
