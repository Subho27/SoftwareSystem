/*
============================================================================
Name : 19d.c
Author : Subhodip Rudra
Description : Create a FIFO file by
    d. mknod system call
Date: 10th Oct, 2023.
============================================================================
*/
#include<stdio.h>
#include<unistd.h>
#include<sys/stat.h>
int main(){
    char* mknodepath = "myfifo-mknode";
	int status = mknod(mknodepath, __S_IFIFO | 0777, 0);
	if(status == -1){
        printf("There was a error creating a Fifo using mknode.\n");
    }
    else{
        printf("Successfully created Fifo using mknode.\n");
    }
	return 0;
}