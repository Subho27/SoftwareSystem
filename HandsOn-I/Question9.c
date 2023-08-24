#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>

int main(int argc, char *argv[])
{
    char *file;
    int status;

    struct stat statbuf;
    if (argc != 2)
        printf("Please pass the file as argument!\n");
    else
    {
        file = argv[1];
        status = stat(file, &statbuf);

        if (status == -1)
            perror("Can not open the file!");
        else
        {
            printf("Inode -> %ld\n", statbuf.st_ino);
            printf("Number of hardlinks -> %ld\n", statbuf.st_nlink);
            printf("UID -> %d\n", statbuf.st_uid);
            printf("GID -> %d\n", statbuf.st_gid);
            printf("Size -> %ld\n", statbuf.st_size);
            printf("Block Size -> %ld\n", statbuf.st_blksize);
            printf("Number of Blocks -> %ld\n", statbuf.st_blocks);
            printf("Time of last access -> %s", ctime(&statbuf.st_atim.tv_sec));
            printf("Time of last modification -> %s", ctime(&statbuf.st_mtim.tv_sec));
            printf("Time of last change -> %s", ctime(&statbuf.st_ctim.tv_sec));
        }
    }
    return 0;
}
