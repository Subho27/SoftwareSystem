#include <stdio.h>      // Import for `printf` & `perror` functions
#include <errno.h>      // Import for `errno` variable
#include <fcntl.h>      // Import for `fcntl` functions
#include <unistd.h>     // Import for `fork`, `fcntl`, `read`, `write`, `lseek, `_exit` functions
#include <sys/types.h>  // Import for `socket`, `bind`, `listen`, `connect`, `fork`, `lseek` functions
#include <sys/socket.h> // Import for `socket`, `bind`, `listen`, `connect` functions
#include <netinet/ip.h> // Import for `sockaddr_in` stucture
#include <string.h>     // Import for string functions

#define PORT 9990

int main() {

    struct sockaddr_in serv;

    int sc = socket(AF_INET, SOCK_STREAM, 0);
    if(sc == -1){
        printf("Socket could not be opened");
        return -1;
    }

    serv.sin_family = AF_INET;
    serv.sin_port = htons(PORT);
    serv.sin_addr.s_addr = htonl(INADDR_ANY);

    int connst = connect(sc, (struct sockaddr *)&serv, sizeof(serv));
    if(connst == -1){
        printf("There is an error while connecting to server.\n");
        return -1;
    }

    char rbuf[1000], wbuf[1000], tbuf[1000];
    int rbyte = 0, wbyte = 0;

    do {
        bzero(rbuf, sizeof(rbuf));
        bzero(tbuf, sizeof(tbuf));

        rbyte = read(sc, rbuf, sizeof(rbuf));
        if(rbyte == -1){
            printf("There is an error while reading from client.\n");
        }
        else if(rbyte == 0){
            printf("No data received from server.\nClosing connection to the server!\n");
        }
        else if(strchr(rbuf, '^') != NULL){
            strncpy(tbuf, rbuf, strlen(rbuf)-1);
            printf("%s\n", tbuf);
            wbyte = write(sc, "^", strlen("^"));
            if(wbyte == -1){
                printf("There is an error while writing to client.\n");
                break;
            }
        }
        else if(strchr(rbuf, '$') != NULL){
            strncpy(tbuf, rbuf, strlen(rbuf)-1);
            printf("%s\n", tbuf);
            printf("Closing connection to the server!\n");
            break;
        }
        else{
            bzero(wbuf, sizeof(wbuf));

            if (strchr(rbuf, '#') != NULL)
                // Taking input without printing it in terminal
                strcpy(wbuf, getpass(rbuf));
            else
            {
                // Taking input while showing it in terminal
                printf("%s\n", rbuf);
                scanf("%[^\n]%*c", wbuf);
            }

            wbyte = write(sc, wbuf, strlen(wbuf));
            if(wbyte == -1){
                perror("There is an error while writing to client socket!\n");
                printf("Closing connection to the server!\n");
                break;
            }
        }
    }while(rbyte > 0);

    close(sc);

    return 0;
}