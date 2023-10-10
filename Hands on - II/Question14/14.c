/*
============================================================================
Name : 14.c
Author : Subhodip Rudra
Description : Write a simple program to create a pipe, write to the pipe, 
	read from pipe and display on the monitor.
Date: 9th Oct, 2023.
============================================================================
*/
#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
int main(){
	int pipefd[2];
    char b[100];
    int st = pipe(pipefd);
    if(st == -1){
        printf("There is a problem in declaring pipe.\n");
        return -1;
    }
    int wbyte = write(pipefd[1], "Hi, I am the first one to write.\n", 33);
	int rbyte = read(pipefd[0], &b, 33);
	if(rbyte > 0){
	    printf("Message from Write end : %s", b);
        printf("Successfully read.\n");
	}
	else{
		printf("There is a problem in reading from file.\n");
        return -1;
	}
	return 0;
}