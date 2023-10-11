/*
============================================================================
Name : 24.c
Author : Subhodip Rudra
Description : Write a program to create a message queue and print the key 
    and message queue id.
Date: 11th Oct, 2023.
============================================================================
*/
#include<sys/msg.h>
#include<stdio.h>
int main(){
    int key = ftok(".", 'a');
    int qid = msgget(key, IPC_CREAT | 0777);
    if(qid <= -1){
        printf("Failed getting message queue.\n");
        return -1;
    }
    printf("Key : %d\nMessage queue id : %d\n", key, qid);
    return 0;
}