#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

int main(int argc, char* argv[]){
        if(argc != 2){
                printf("You have given wrong number of arguments\n");
                return 0;
        }
        int fd_rdwr = open(argv[1],O_RDWR);
        if(fd_rdwr == -1){
                printf("You doesn't know how to intialize a file descriptor\n");
                return 0;
        }
        printf("File descriptor for reading and writing is %d \n",fd_rdwr);
        int fd_rdwr_close = close(fd_rdwr);
        if(fd_rdwr_close == -1){
                printf("Your system doesn't know to close file descriptor\n");
                return 0;
        }
        return 0;
}
