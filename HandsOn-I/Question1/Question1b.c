#include <unistd.h> 
#include <stdio.h>

void main(int argc, char *argv[])
{

    char *filePath; 
    char *hardlinkPath;

    int status;

    if (argc != 3)
        printf("Enter File path and Hardlink File path as the arguments to the program\n");
    else
    {
        filePath = argv[1];
        hardlinkPath = argv[2];

        status = link(filePath, hardlinkPath);

        if (status == -1)
            perror("Error while creating hard link!");
        else
            printf("Successfully created a hard link. Check with the `ll` or `ls -l` command.\n");
    }
}
