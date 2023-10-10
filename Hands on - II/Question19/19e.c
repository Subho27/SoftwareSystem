/*
============================================================================
Name : 19e.c
Author : Subhodip Rudra
Description : Create a FIFO file by
    e. mkfifo library function
Date: 10th Oct, 2023.
============================================================================
*/
#include<stdio.h>
#include<unistd.h>
#include<sys/stat.h>
int main(){
	char* mkfifopath = "myfifo-mkfifo";
    int status1 = mkfifo(mkfifopath, 0777);
    if(status1 == -1){
        printf("There was a error creating a Fifo using mkfifo.\n");
    }
    else{
        printf("Successfully created Fifo using mkfifo.\n");
    }
	return 0;
}