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
int main(int argc, char* args[]){
	char* filename;
	char b[100];
	if(argc != 2){
		printf("Please enter a filename to use.\n");
	}
	else{
		filename = args[1];
		int fd = open(filename, O_RDONLY);
		int rbyte = read(fd, b, 33);
		if(rbyte > 0){
			int i = 0;
			while(i < 33){
				printf("%c", b[i]);
				i++;
			}
			printf("Successfully read.\n");
			close(fd);
		}
		else{
			printf("There is a problem in reading from file.\n");
		}
	
	}
	return 0;
}
