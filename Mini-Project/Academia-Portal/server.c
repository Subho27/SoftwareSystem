#include <stdio.h>      // Import for `printf` & `perror` functions
#include <errno.h>      // Import for `errno` variable
#include <fcntl.h>      // Import for `fcntl` functions
#include <unistd.h>     // Import for `fork`, `fcntl`, `read`, `write`, `lseek, `_exit` functions
#include <sys/types.h>  // Import for `socket`, `bind`, `listen`, `accept`, `fork`, `lseek` functions
#include <sys/socket.h> // Import for `socket`, `bind`, `listen`, `accept` functions
#include <netinet/ip.h> // Import for `sockaddr_in` stucture
#include <string.h>     // Import for string functions
#include <stdbool.h>    // Import for `bool` data type
#include <stdlib.h>     // Import for `atoi` function

// Import handlers
#include "./handler/admin-handler.h"
#include "./handler/faculty-handler.h"
#include "./handler/student-handler.h"

#define PORT 9990       // define port number 

int main(){
    struct sockaddr_in serv, clie;

    // Createing socket fd with socket()
    int sc = socket(AF_INET, SOCK_STREAM, 0);
    if(sc < 0){
        printf("Socket could not be opened");
        return -1;
    }

    // Setting values in socket address struct
    serv.sin_family = AF_INET;
    serv.sin_port = htons(PORT);
    serv.sin_addr.s_addr = htonl(INADDR_ANY);

    // Binding socket fd with socket address struct
    int bindstatus = bind(sc, (struct sockaddr*)&serv, sizeof(serv));
    if(bindstatus == -1){
        printf("There is an error while binding.\n");
        return -1;
    }

    // Start listening in socket
    int listenstatus = listen(sc, 10);
    if(listenstatus == -1){
        printf("There is an error while listening.\n");
        close(sc);
        return -1;
    }

    while(1){
        int clientSize = (int)sizeof(clie);
        // Accepting client connections
        int connfd = accept(sc, (struct sockaddr *)&clie, &clientSize);
        if(connfd == -1){
            printf("There is an error while connecting to client.\n");
            close(sc);
        }
        else{
            if(!fork()){
                char rBuf[1000], wBuf[1000];
                //char prompt[] = "Welcome!\nPlease enter";
                int wbytes = write(connfd, INITIAL_PROMPT, sizeof(INITIAL_PROMPT));
                if(wbytes == -1){
                    printf("There is an error while sending initial prompt.\n");
                }
                else{
                    bzero(rBuf, sizeof(rBuf));
                    int rbytes = read(connfd, rBuf, sizeof(rBuf));
                    if(rbytes == -1){
                        printf("There is an error while reading from client.\n");
                    }
                    else if(rbytes == 0){
                        printf("No data received from client.\n");
                    }
                    else{
                        int choice  = atoi(rBuf);
                        switch (choice)
                        {
                        case 1: 
                            admin_op_handler(connfd);
                            break;
                        case 2:
                            faculty_op_handler(connfd);
                            break;
                        case 3:
                            student_op_handler(connfd);
                            break;
                        default:
                            printf("Please provide correct input :(");
                            break;
                        }
                    }
                }
                printf("Terminating connection to client!\n");
            }
        }
    }
    return 0;
}