#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/stat.h>
int main(int argc, char *args[]) {
	char *filename;
	struct stat fileStat;
	if(argc != 2){
		printf("Please enter a filename to read information.\n");
		return 0;
	}
	else{
		filename = args[1];
		stat(filename, &fileStat);
		printf("1. ID of device containing file : %ld\n", fileStat.st_dev);
		printf("2. Inode Number : %ld\n", fileStat.st_ino);
		printf("3. File type and mode : %d\n", fileStat.st_mode);
		printf("4. Number of hardlinks : %ld\n", fileStat.st_nlink);
		printf("5. User ID of owner : %d\n", fileStat.st_uid);
		printf("6. Group ID of owner : %d\n", fileStat.st_gid);
		printf("7. Device ID (If special file) : %ld\n", fileStat.st_rdev);
		printf("8. Total size in bytes : %ld\n", fileStat.st_size);
		printf("9. Block size for file system I/O : %ld\n", fileStat.st_blksize);
		printf("10. Number of 512B blocks allocated : %ld\n", fileStat.st_blocks);
		printf("11. Last access time : %ld\n", fileStat.st_atime);
		printf("12. Last Modification time : %ld\n", fileStat.st_mtime);
		printf("13. Last status change time : %ld\n", fileStat.st_ctime);
	}
	return 0;
}

//Same file information can be achieved by "stat filename" command.
