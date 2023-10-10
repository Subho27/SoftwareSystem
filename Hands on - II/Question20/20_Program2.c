/*
============================================================================
Name : 20.c
Author : Subhodip Rudra
Description : Write two programs so that both can communicate by FIFO -Use 
    one way communication.
Date: 10th Oct, 2023.
============================================================================
*/
#include<stdio.h>
#include<unistd.h>
#include<sys/stat.h>
#include<fcntl.h>
int main() {
    char* filename = "demo";
    int fd, pid;
    char b[100];
    int status_cr = mkfifo(filename, 0777);
    if(status_cr == -1){
        printf("Program 2 : Already file created.\n");
    }
    fd = open(filename, O_RDONLY);
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