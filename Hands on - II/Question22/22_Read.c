/*
============================================================================
Name : 22.c
Author : Subhodip Rudra
Description : Write a program to wait for data to be written into FIFO 
    within 10 seconds, use select system call with FIFO.
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
    char* filename = "demo";
    char b[100];
    int fd;
    fd_set readfds;
    struct timeval timer;
    int status_cr = mkfifo(filename, 0777);
    if(status_cr == -1){
        printf("Already file created.\n");
    }
    fd = open(filename, O_NONBLOCK | O_RDONLY);
    FD_ZERO(&readfds);
    FD_SET(fd, &readfds);

    timer.tv_sec = 10;
    timer.tv_usec = 0;

    int st = select(fd+1, &readfds, NULL, NULL, &timer);
    if(st == -1){
        printf("Error while executing select call\n");
        return -1;
    }
    else if(st){
        int rbyte = read(fd, &b, 6);
        if(rbyte < 0){
            printf("Read from pipe failed.\n");
            return -1;
        }
        printf("%s", b);
    }
    else{
        printf("No data received.\n");
    }
    close(fd);
    return 0;
}