/*
============================================================================
Name : 33.c
Author : Suraj Subedi
Description : Write a program to communicate between two machines using socket.
Date: 9th Oct, 2023.
============================================================================
*/

#include <sys/types.h>  
#include <sys/socket.h> 
#include <netinet/ip.h> 
#include <stdio.h>      
#include <unistd.h>  

int main()
{
    struct sockaddr_in serv;
    int sd;
    char buff[80];
    sd=socket(AF_UNIX,SOCK_STREAM,0);
    serv.sin_family=AF_UNIX;

    serv.sin_addr.s_addr=INADDR_ANY;

    serv.sin_port=htons(3368);

    connect(sd,(void*)(&serv),sizeof(serv));

    write(sd,"Hello Server\n",17);

    read(sd,buff,sizeof(buff));

    printf("Messege from server: %s\n",buff);




}