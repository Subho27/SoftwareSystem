/*
============================================================================
Name : 16.c
Author : Subhodip Rudra
Description : Write a program to send and receive data from parent to child 
    vice versa. Use two way communication.
Date: 9th Oct, 2023.
============================================================================
*/
#include<stdio.h>
#include<unistd.h>
#include<sys/stat.h>
#include<fcntl.h>
int main() {
    int pipe1_fd[2], pipe2_fd[2], pid;
    int status_cr = pipe(pipe1_fd);
    if(status_cr == -1){
        printf("There is a problem in declaring pipe.\n");
        return -1;
    }
    status_cr = pipe(pipe2_fd);
    if(status_cr == -1){
        printf("There is a problem in declaring pipe.\n");
        return -1;
    }
    pid = fork();
    if(pid > 0){
        close(pipe1_fd[0]);
        close(pipe2_fd[1]);
        char b[100];
        printf("Parent : Hello I am parent.\nParent : I have started writing to first pipe.\n");
        int wbyte = write(pipe1_fd[1], "***This is a message from parent***\n", 36);
        if(wbyte < 0){
            printf("Parent : Write to first pipe failed.\n");
            return -1;
        }
        printf("Parent : I am done writing to first pipe.\n");
        int rbyte = read(pipe2_fd[0], &b, 35);
        printf("Parent : Reading of child message done.\n");
        printf("%s", b);
        printf("Parent : I have got your message, Child.\n");
    }
    else{
        close(pipe1_fd[1]);
        close(pipe2_fd[0]);
        char b[100];
        printf("Child : Hello I am child.\nChild : I have got a message from parent.\n");
        int rbyte = read(pipe1_fd[0], &b, 36);
        printf("Child : Reading of parent message done.\n");
        printf("%s", b);
        printf("Child : I have got your message, Parent.\n");
        printf("Child : I have started writing to second pipe.\n");
        int wbyte = write(pipe2_fd[1], "***This is a message from child***\n", 35);
        if(wbyte < 0){
            printf("Child : Write to second pipe failed.\n");
            return -1;
        }
        printf("Child : I am done writing to second pipe.\n");
    }
    return 0;
}