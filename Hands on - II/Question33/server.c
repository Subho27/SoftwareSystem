/*
============================================================================
Name : 33.c
Author : Subhodip Rudra
Description : Write a program to communicate between two machines using socket.
Date: 14th Oct, 2023.
============================================================================
*/

#include <sys/types.h>  
#include <sys/socket.h> 
#include <netinet/ip.h> 
#include <stdio.h>      
#include <unistd.h>     

int main()
{
    struct sockaddr_in serv,cli;
    int sd,nsd;
    char buff[80];
    sd=socket(AF_UNIX,SOCK_STREAM,0);

    serv.sin_family=AF_UNIX;

    serv.sin_addr.s_addr=INADDR_ANY;

    serv.sin_port=htons(3368);

    bind(sd,(void*)(&serv),sizeof(serv));

    listen(sd,5);
    int sz=sizeof(cli);

    nsd=accept(sd,(void*) (&cli),&sz);

    read (nsd,buff, sizeof(buff));

    printf("Message from client: %s\n", buff);
    write(nsd,"ACK from Server\n",17);



}