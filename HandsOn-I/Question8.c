#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <unistd.h>
#include <stdio.h>
int main(int argc, char *argv[])
{

    char *file;
    int fD;
    char b[1];

    if (argc != 2)
        printf("Please pass the file as argument.");
    else
    {
        file = argv[2];
        fD = open(file, O_RDONLY);

        if (fD == -1)
            perror("Cannot open the file!");
        else
        {
            while (read(fD, b, 1) == 1)
            {
                if (b[0] == '\n')
                    write(STDOUT_FILENO, "\n\n", 2);
                else
                    write(STDOUT_FILENO, b, 1);
            }
            write(STDOUT_FILENO, "\n", 1);
            close(fD);
        }
    }
    return 0;
}
