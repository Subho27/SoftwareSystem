#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h>
#include <unistd.h> 
#include <stdio.h>

void main(int argc, char *argv[])
{

    char *filename;             
    int fileDescriptor, dupFileDescriptor; 
    int writeByteCount;   

    if (argc != 2)
        printf("Pass the file name as the argument!");
    else
    {
        filename = argv[1];
        fileDescriptor = open(filename, O_WRONLY | O_APPEND);
        if (fileDescriptor == -1)
            perror("Error while opening file");
        else
        {
            dupFileDescriptor = fcntl(fileDescriptor, F_DUPFD);

            writeByteCount = write(fileDescriptor, "Using original FD", 17);
            if (writeByteCount == -1)
                perror("Error while writing file using original FD");

            writeByteCount = write(dupFileDescriptor, "Using duplicate FD", 18);
            if (writeByteCount == -1)
                perror("Error while writing file using duplicate FD");

            close(fileDescriptor);
        }
    }
}
