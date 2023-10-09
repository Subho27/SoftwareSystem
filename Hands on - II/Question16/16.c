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
    char* filename1 = "demo1";
    char* filename2 = "demo2";
    int fd[2], pid;
    int status_cr = mkfifo(filename1, 0777);
    if(status_cr == -1){
        printf("Already file created.\n");
    }
    status_cr = mkfifo(filename2, 0777);
    if(status_cr == -1){
        printf("Already file created.\n");
    }
    pid = fork();
    if(pid > 0){
        close(fd[0]);
        char b[100];
        printf("Parent : Hello I am parent.\nParent : I have started writing to first pipe.\n");
        fd[0] = open(filename1, O_WRONLY);
        int wbyte = write(fd[0], "***This is a message from parent***\n", 36);
        if(wbyte < 0){
            printf("Parent : Write to first pipe failed.\n");
        }
        printf("Parent : I am done writing to first pipe.\n");
        close(fd[0]);
        fd[1] = open(filename2, O_RDONLY);
        printf("Parent : Reading of child message done.\n");
        int rbyte = read(fd[1], &b, 35);
        for(int i=0; i<35; i++){
            printf("%c", b[i]);
        }
        printf("Parent : I have got your message, Child.\n");
        close(fd[1]);
    }
    else{
        close(fd[0]);
        char b[100];
        printf("Child : Hello I am child.\nChild : I have got a message from parent.\n");
        fd[0] = open(filename1, O_RDONLY);
        printf("Child : Reading of parent message done.\n");
        int rbyte = read(fd[0], &b, 36);
        for(int i=0; i<36; i++){
            printf("%c", b[i]);
        }
        printf("Child : I have got your message, Parent.\n");
        close(fd[0]);
        printf("Child : I have started writing to second pipe.\n");
        fd[1] = open(filename2, O_WRONLY);
        int wbyte = write(fd[1], "***This is a message from child***\n", 35);
        if(wbyte < 0){
            printf("Child : Write to second pipe failed.\n");
        }
        printf("Child : I am done writing to second pipe.\n");
        close(fd[1]);
    }
    return 0;
}