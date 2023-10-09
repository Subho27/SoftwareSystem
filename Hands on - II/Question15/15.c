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
    char* filename = "demo";
    int fd, pid;
    int status_cr = mkfifo(filename, 0777);
    if(status_cr == -1){
        printf("Already file created.\n");
    }
    pid = fork();
    if(pid > 0){
        close(fd);
        printf("Parent : Hello I am parent.\nParent : I have started writing to pipe.\n");
        fd = open(filename, O_WRONLY);
        int wbyte = write(fd, "***This is a message from parent***\n", 36);
        if(wbyte < 0){
            printf("Parent : Write to pipe failed.\n");
        }
        printf("Parent : I am done writing to pipe.\n");
        close(fd);
    }
    else{
        close(fd);
        char b[100];
        printf("Child : Hello I am child.\nChild : I have got a message from parent.\n");
        fd = open(filename, O_RDONLY);
        printf("Child : Reading of parent message done.\n");
        int rbyte = read(fd, &b, 36);
        for(int i=0; i<36; i++){
            printf("%c", b[i]);
        }
        printf("Child : I have got your message, Parent.\n");
        close(fd);
    }
    return 0;
}