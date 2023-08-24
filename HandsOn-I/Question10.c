#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

void main(int argc, char *argv[])
{
    char *filename;
    int fileDescriptor;
    int offset;
    int writeByteCount;

    if (argc != 2)
        printf("Please pass file as the argument!");
    else
    {
        filename = argv[1];
        fileDescriptor = open(filename, O_RDWR);

        if (fileDescriptor == -1)
            perror("Can not open file! ");
        else
        {
            offset = lseek(fileDescriptor, 10, SEEK_SET);
            writeByteCount = write(fileDescriptor, "ABCDEFGHIJ", 10);
            if (writeByteCount == -1)
            {
                perror("Can not write to the file!");
                _exit(1);
            }

            offset = lseek(fileDescriptor, 10, SEEK_CUR);
            writeByteCount = write(fileDescriptor, "ABCDEFGHIJ", 10);
            if (writeByteCount == -1)
            {
                perror("Can not write to the file!");
                _exit(1);
            }

            printf("Return value of lseek: %d\n", offset);
            close(fileDescriptor);
        }
    }
}
