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
#include<fcntl.h>
int main() {
    char* filename = "demo";
    int fd;
    int status_cr = mkfifo(filename, 0777);
    if(status_cr == -1){
        printf("Already file created.\n");
    }
    fd = open(filename, O_NONBLOCK | O_WRONLY);
    // Provide amount of time you want this program to sleep
    // if value larger than 10 read program will not read
    sleep(6);
    write(fd, "Hello!", 6);
    close(fd);
    return 0;
}