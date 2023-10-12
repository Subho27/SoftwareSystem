/*
============================================================================
Name : 26.c
Author : Subhodip Rudra
Description : Write a program to send messages to the message queue. 
    Check $ipcs -q
Date: 12th Oct, 2023.
============================================================================
*/
#include<sys/msg.h>
#include<stdio.h>
#include<string.h>
struct msg
{
    long int m_type;
    char message[80];
} msq;
int main(){
    int key = ftok(".", 'a');
    int msqqid = msgget(key, IPC_CREAT | 0777);
    printf("Enter message type : ");
    scanf("%ld", &msq.m_type);
    printf("Enter message text : ");
    scanf(" %[^\n]", msq.message);
    int size = strlen(msq.message);
    int sts = msgsnd(msqqid, &msq, size+1, 0);
    if(sts <= -1){
        printf("Message could not be sent.\n");
    }
    return 0;
}
