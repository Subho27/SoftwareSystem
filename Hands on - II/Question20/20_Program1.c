/*
============================================================================
Name : 20.c
Author : Subhodip Rudra
Description : Write two programs so that both can communicate by FIFO -Use 
    one way communication.
Date: 10th Oct, 2023.
============================================================================
*/
#include<stdio.h>
#include<unistd.h>
#include<sys/stat.h>
#include<fcntl.h>
int main() {
    char* filename = "demo";
    int fd, pid;
    int status_cr = mkfifo(filename, 0777);
    if(status_cr == -1){
        printf("Program 1 : Already file created.\n");
    }
    fd = open(filename, O_WRONLY);
    int wbyte = write(fd, "***This is a message from Program 1***\n", 39);
    if(wbyte < 0){
        printf("Program 1 : Write to pipe failed.\n");
        return 0;
    }
    printf("Program 1 : I am done writing to pipe.\n");
    close(fd);
    return 0;
}