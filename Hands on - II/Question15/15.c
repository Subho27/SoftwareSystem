/*
============================================================================
Name : 15.c
Author : Subhodip Rudra
Description : Write a simple program to send some data from parent to 
    the child process.
Date: 9th Oct, 2023.
============================================================================
*/
#include<stdio.h>
#include<unistd.h>
#include<sys/stat.h>
#include<fcntl.h>
int main() {
    int fd[2], pid;
    int status_cr = pipe(fd);
    if(status_cr == -1){
        printf("There is a problem in declaring pipe.\n");
        return -1;
    }
    pid = fork();
    if(pid > 0){
        close(fd[0]); // Closing read end of pipe
        printf("Parent : Hello I am parent.\nParent : I have started writing to pipe.\n");
        int wbyte = write(fd[1], "***This is a message from parent***\n", 36);
        if(wbyte < 0){
            printf("Parent : Write to pipe failed.\n");
            return -1;
        }
        printf("Parent : I am done writing to pipe.\n");
    }
    else{
        close(fd[1]); // Closing write end of pipe
        char b[100];
        printf("Child : Hello I am child.\nChild : I have got a message from parent.\n");
        int rbyte = read(fd[0], &b, 36);
        printf("Child : Reading of parent message done.\n");
        printf("%s", b);
        printf("Child : I have got your message, Parent.\n");
    }
    return 0;
}