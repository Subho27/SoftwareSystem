/*
============================================================================
Name : 19c.c
Author : Subhodip Rudra
Description : Create a FIFO file by
    c. use strace command to find out, which command (mknod or mkfifo) is 
    better.
Date: 10th Oct, 2023.
============================================================================
*/

// 1. Full command looks like : "strace -o mknod-fifo.txt mknod fifofile p"
// 2. Full command looks like : "strace -o mkfifo-fifo.txt mkfifo fifofile"
// 3. Now we can compare these 2 command to find out difference
// 4. Strace system call shows us the system calls they use to execute the task
// 5. If we see carefully, mkfifo actually calls mknod in its execution path
// 6. so we can say mknod(system call) is faster than mkfifo(library function)
