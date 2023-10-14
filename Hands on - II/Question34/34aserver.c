/*
============================================================================
Name : 34aserver.c
Author : Suraj Subedi
Description : Write a program to create Concurrent Server.Using fork
Date: 9th Oct, 2023.
============================================================================
*/


#include <sys/types.h>  // Import for `socket`, `bind`, `listen`, `accept`, `fork`
#include <sys/socket.h> // Import for `socket`, `bind`, `listen`, `accept`
#include <netinet/ip.h> // Import for `struct sockaddr_in`, `htons`
#include <stdio.h>      // Import for `perror` & `printf`
#include <unistd.h>     // Import for `_exit`, `read`, `write`, `fork`

void main()
{
    int socketFileDescriptor, connectionFileDescriptor;
    int bindStatus;
    int listenStatus;
    int clientSize;

    struct sockaddr_in address, client;

    ssize_t readBytes, writeBytes;
    char dataFromClient[100];

    socketFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFileDescriptor == -1)
    {
        perror("Error while creating socket!");
        _exit(0);
    }
    printf("Server side socket successfully created!\n");

    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_family = AF_INET;
    address.sin_port = htons(8080);

    bindStatus = bind(socketFileDescriptor, (struct sockaddr *)&address, sizeof(address));
    if (bindStatus == -1)
    {
        perror("Error while binding name to socket!");
        _exit(0);
    }
    printf("Binding to socket was successful!\n");

    listenStatus = listen(socketFileDescriptor, 2);
    if (listenStatus == -1)
    {
        perror("Error while trying to listen for connections!");
        _exit(0);
    }
    printf("Now listening for connections on a socket!\n");
    while (1)
    {
        clientSize = (int)sizeof(client);
        connectionFileDescriptor = accept(socketFileDescriptor, (struct sockaddr *)&client, &clientSize);
        if (connectionFileDescriptor == -1)
            perror("Error while accepting a connection!");
        else
        {

            // ========================= Server - Client communication =================

            if (fork() == 0)
            {
                // Child
                writeBytes = write(connectionFileDescriptor, "I'm the server!", 15);
                if (writeBytes == -1)
                    perror("Error while writing to network via socket!");
                else
                    printf("Data sent to client!\n");

                readBytes = read(connectionFileDescriptor, dataFromClient, 100);
                if (readBytes == -1)
                    perror("Error while reading from network via socket!");
                else
                    printf("Data from client: %s\n", dataFromClient);
            }
            else
            {
                // Parent
                close(connectionFileDescriptor);
            }
        }

        // =======================================================================
    }

    close(socketFileDescriptor);
}