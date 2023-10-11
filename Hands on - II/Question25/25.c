/*
============================================================================
Name : 25.c
Author : Subhodip Rudra
Description : Write a program to print a message queue's (use msqid_ds and 
    ipc_perm structures)
    a. access permission
    b. uid, gid
    c. time of last message sent and received
    d. time of last change in the message queue
    d. size of the queue
    f. number of messages in the queue
    g. maximum number of bytes allowed
    h. pid of the msgsnd and msgrcv
Date: 11th Oct, 2023.
============================================================================
*/
#include<sys/msg.h>
#include<stdio.h>
int main(){
    struct msqid_ds msgqstat; 
    int key = ftok(".", 'a');
    int qid = msgget(key, IPC_CREAT | 0777);
    if(qid <= -1){
        printf("Failed getting message queue.\n");
        return -1;
    }
    printf("Key : %d\nMessage queue id : %d\n", key, qid);
    int status_cr = msgctl(qid, IPC_STAT, &msgqstat);
    if(status_cr < -1){
        printf("Failed getting message queue status.\n");
        return -1;
    }
    printf("Message queue details : \n");
    printf("Access Permission : %d\n", msgqstat.msg_perm.mode);
    printf("Owner's user id : %d\n", msgqstat.msg_perm.uid);
    printf("Owner's group id : %d\n", msgqstat.msg_perm.gid);
    printf("Creator's user id : %d\n", msgqstat.msg_perm.cuid);
    printf("Creator's group id : %d\n", msgqstat.msg_perm.cgid);
    printf("Time of last message sent : %ld\n", msgqstat.msg_stime);
    printf("Time of last message received : %ld\n", msgqstat.msg_rtime);
    printf("Time of last change in the message queue : %ld\n", msgqstat.msg_ctime);
    printf("Size of the queue : %ld\n", msgqstat.__msg_cbytes);
    printf("Number of messages in the queue : %ld\n", msgqstat.msg_qnum);
    printf("Maximum number of bytes allowed : %ld\n", msgqstat.msg_qbytes);
    printf("PID of the msgsnd : %d\n", msgqstat.msg_lspid);
    printf("PID of the msgrcv : %d\n", msgqstat.msg_lrpid);
    return 0;
}