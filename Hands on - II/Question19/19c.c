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
// 4. POSIX specification (set of rules for UNIX based operating system) recommends to use mkfifo
// 5. mknod can be used for other type of files too.
// 6. Apart for those there is no other difference.