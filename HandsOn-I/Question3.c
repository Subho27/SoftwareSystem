#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
int main(int argc, char *argv[])
{
    char *file;
    int fileD;

    if (argc != 2)
        printf("Please pass filename as argument.\n");
    else
    {
        file = argv[1];

        fileD = creat(file, S_IRWXU);

        if (fileD == -1)
            perror("File can not be created!");
        else
            printf("Successfully created : File descriptor value -> %d\n", fileD);
    }
    return 0;
}
