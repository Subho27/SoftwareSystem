#include <unistd.h>
#include <stdio.h>
int main()
{
    char *b[100];
    int rByte, wByte;
    while (1)
    {
        rByte = read(STDIN_FILENO, b, 1);
        wByte = write(STDOUT_FILENO, b, 1);

        if (rByte == -1 || wByte == -1)
        {
            perror("Fatal Error : while reading or writing");
            break;
        }
    }
    return 0;
}
