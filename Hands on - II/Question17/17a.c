/*
============================================================================
Name : 17a.c
Author : Subhodip Rudra
Description : Write a program to execute ls -l | wc.
    a. use dup
Date: 9th Oct, 2023.
============================================================================
*/
#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/stat.h>
int main(){
    char *filename = "demo";
    int fd;
    int status_cr = mkfifo(filename, 0777);
    if(status_cr < 0){
        printf("Already FIFO file created.\n");
    }
    int pid = fork();
    if(pid == 0){
        // printf("Opening FIFO in write only access.\n");
        fd = open(filename, O_WRONLY);
        // printf("Closing standard output file descriptor.\n");
        close(STDOUT_FILENO);
        // printf("Assign FIFO file fd to default STDOUT fd.\n");
        dup(fd);
        // printf("Executing ls -l cmd and returning it to FIFO fd (instead of STDOUT).\n");
        execl("/usr/bin/ls", "ls -l", "-l", NULL);
        close(fd);
    }
    else{
        // printf("Opening FIFO in read only access.\n");
        fd = open(filename, O_RDONLY);
        // printf("Closing standard input file descriptor.\n");
        close(STDIN_FILENO);
        // printf("Assign FIFO file fd to default STDIN fd.\n");
        dup(fd);
        // printf("Executing wc cmd while taking input from FIFO file fd (instead of STDIN).\n");
        execl("/usr/bin/wc", "wc", NULL);
        close(fd);
    }
    return 0;
}