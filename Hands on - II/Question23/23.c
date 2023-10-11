/*
============================================================================
Name : 23.c
Author : Subhodip Rudra
Description : Write a program to print the maximum number of files can be 
    opened within a process and size of a pipe (circular buffer).
Date: 11th Oct, 2023.
============================================================================
*/
#include<stdio.h>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<sys/time.h>
#include<fcntl.h>
int main() {
    char *filename = "demo";
    int fd;
    long maxsize, sizeofpipe;
    int status_cr = mkfifo(filename, 0777);
    if(status_cr == -1){
        printf("Already created\n");
    }
    maxsize = sysconf(_SC_OPEN_MAX);
    if(maxsize == -1){
        printf("There is an error calling sysconf.\n");
    }
    else{
        printf("Maximum number of files that can be opened : %ld\n", maxsize);
    }
    sizeofpipe = pathconf(filename, _PC_PIPE_BUF);
    if(sizeofpipe == -1){
        printf("There is an error calling pathconf.\n");
    }
    else{
        printf("Maximum size of pipe : %ld\n", sizeofpipe);
    }
    return 0;
}