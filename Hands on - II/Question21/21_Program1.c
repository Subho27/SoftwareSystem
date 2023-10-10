/*
============================================================================
Name : 21.c
Author : Subhodip Rudra
Description : Write two programs so that both can communicate by FIFO -Use 
    two way communications.
Date: 10th Oct, 2023.
============================================================================
*/
#include<stdio.h>
#include<unistd.h>
#include<sys/stat.h>
#include<fcntl.h>
int main() {
    char* filename1 = "demo";
    char* filename2 = "demo1";
    char b[100];
    int fd, pid;
    int status_cr = mkfifo(filename1, 0777);
    if(status_cr == -1){
        printf("Program 1 : Already file created.\n");
    }
    status_cr = mkfifo(filename2, 0777);
    if(status_cr == -1){
        printf("Program 1 : Already file created.\n");
    }
    fd = open(filename1, O_WRONLY);
    int wbyte = write(fd, "***This is a message from Program 1***\n", 39);
    if(wbyte < 0){
        printf("Program 1 : Write to pipe failed.\n");
        return 0;
    }
    printf("Program 1 : I am done writing to pipe.\n");
    close(fd);
    fd = open(filename2, O_RDONLY);
    int rbyte = read(fd, &b, 39);
    if(rbyte < 0){
        printf("Program 2 : read from pipe failed.\n");
        return 0;
    }
    for(int i=0; i<39; i++){
        printf("%c", b[i]);
    }
    printf("Program 2 : I am done reading from pipe.\n");
    close(fd);
    return 0;
}