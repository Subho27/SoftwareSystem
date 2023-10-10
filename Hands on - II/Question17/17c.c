/*
============================================================================
Name : 17c.c
Author : Subhodip Rudra
Description : Write a program to execute ls -l | wc.
    c. use fcntl
Date: 9th Oct, 2023.
============================================================================
*/
#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/stat.h>
int main(){
    int fd[2];
    int status_cr = pipe(fd);
    if(status_cr < 0){
        printf("There is a problem in declaring pipe.\n");
        return -1;
    }
    int pid = fork();
    if(pid == 0){
        close(STDOUT_FILENO);
        close(fd[0]);
        fcntl(fd[1], F_DUPFD, STDOUT_FILENO);
        execl("/usr/bin/ls", "ls -l", "-l", NULL);
    }
    else{
        close(STDIN_FILENO);
        close(fd[1]);
        fcntl(fd[0], F_DUPFD, STDIN_FILENO);
        execl("/usr/bin/wc", "wc", NULL);
    }
    return 0;
}