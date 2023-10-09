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
#include<sys/stat.h>
#include<fcntl.h>
int main(int argc, char* args[]){
	char* filename;
	if(argc != 2){
		printf("Please enter a filename to use.\n");
	}
	else{
		filename = args[1];
		int status = mkfifo(filename, 0777);
		if(status != 1){
			int fd = open(filename, O_WRONLY);
			int wbyte = write(fd, "Hi, I am the first one to write.\n", 33);
			if(wbyte > 0){
				printf("Successfully written.\n");
			}
			else{
				printf("There is a problem in writing to file.\n");
			}
		}
		else{
			printf("There is a problem with Fifo Creation.\n");
		}
	}
	return 0;
}
