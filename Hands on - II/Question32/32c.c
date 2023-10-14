/*
============================================================================
Name : 32c.c
Author : Subhodip Rudra
Description : Write a program to implement semaphore to protect any critical section - Remove the created semaphore
Date: 14th Oct, 2023.
============================================================================
*/

#include <sys/types.h> 
#include <sys/ipc.h>  
#include <sys/sem.h>   
#include <unistd.h>    
#include <stdio.h>     

void main()
{
    key_t semKey;
    int semIndentifier;
    int semctlStatus;

    semKey = ftok(".", 1);
    if (semKey == -1)
    {
        perror("Error while computing key!");
        _exit(1);
    }

    semIndentifier = semget(semKey, 1, IPC_CREAT | 0777);

    if (semIndentifier == -1)
    {
        perror("Error while creating semaphore!");
        _exit(1);
    }

    printf("Press enter to delete the semaphore!\n");
    getchar();

    semctlStatus = semctl(semIndentifier, 0, IPC_RMID);

    if (semctlStatus == -1)
    {
        perror("Error while deleting semaphore!");
        _exit(0);
    }
}