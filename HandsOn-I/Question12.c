#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h> 
#include <stdio.h>

void main(int argc, char *argv[])
{
    char *filename;
    int fileDescriptor;
    int status; 
    if (argc != 2)
        printf("Pass the file to be opened as the argument!\n");
    else
    {
        filename = argv[1];

        fileDescriptor = open(filename, O_RDONLY);

        if (fileDescriptor == -1)
            perror("Error while opening the file");
        else
        {
            status = fcntl(fileDescriptor, F_GETFL);

            if (status == -1)
                perror("Error on executing fcntl!");
            else
            {
                switch (O_ACCMODE & status)
                {
                case 0:
                    printf("The file has been opened with the flags : O_RDONLY\n");
                    break;
                case 1:
                    printf("The file has been opened with the flags : O_WRONLY\n");
                    break;
                case 2:
                    printf("The file has been opened with the flags : O_RDWR\n");
                    break;
                // Any of the below cases will never be invoked
                case 64:
                    printf("The file has been opened with the flags : O_CREAT\n");
                    break;
                case 512:
                    printf("The file has been opened with the flags : O_TRUNC\n");
                    break;
                case 1024:
                    printf("The file has been opened with the flags : O_APPEND\n");
                    break;
                case 577:
                    printf("The file has been opened with the flags : O_WRONLY | O_CREAT | O_TRUNC\n");
                    break;
                case 1089:
                    printf("The file has been opened with the flags : O_WRONLY | O_CREAT | O_APPEND\n");
                    break;
                case 578:
                    printf("The file has been opened with the flags : O_RDWR   | O_CREAT | O_TRUNC\n");
                    break;
                case 1090:
                    printf("The file has been opened with the flags : O_RDWR   | O_CREAT | O_APPEND\n");
                    break;
                default:
                    printf("Error!");
                }
            }
            close(fileDescriptor);
        }
    }
}
