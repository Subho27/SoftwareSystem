#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
int main(int argc, char *args[]){
	char *filename;
	if(argc != 2){
		printf("Please enter a file name.\n");
	}
	else{
		filename = args[1];
		int fd = open(filename, O_CREAT | O_RDWR | O_APPEND, 0777);
		int wbyte = write(fd, "I have been added by old FD.\n", 29);
		printf("Old File Descriptor : %d\n", fd);
		if(wbyte == -1){
			printf("There was a error in writinf with old fd.\n");
		}
		int fd2 = fcntl(fd, F_DUPFD);
		printf("New File Descriptor : %d\n", fd2);
		wbyte = write(fd2, "I have been added by new FD.\n", 29);
                if(wbyte == -1){
                        printf("There was a error in writinf with new fd.\n");
                }
	}
	return 0;
}
