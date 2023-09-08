#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
int main(int argc, char* args[]){
	char* filename;
	if(argc != 2){
		printf("Please enter a filename.\n");
	}
	else{
		filename = args[1];
		int fd = open(filename, O_CREAT | O_WRONLY, 0777);
		int pid = getpid();
		printf("parent has started writing.\n");
		int wbyte = write(fd, "Hi I am from Parent.", 20);
		if(wbyte == -1){
			printf("fatal error while writing from parent.");
		}
		printf("parent has stopped.\n");
		close(fd);
		int cpid = fork();
		if(cpid == 0){
			filename = args[1];
			printf("child has started writing.\n");
			int fd1 = open(filename, O_CREAT | O_WRONLY | O_APPEND, 0777);
			int wbyte1 = write(fd1, "\nHi I am from Child.", 21);
                	if(wbyte1 == -1){
                        	printf("fatal error while writing from child.");
                	}
			printf("child has stopped.\n");
			close(fd);
		}
		else{
			printf("I am parent. I can not write again.\n");
		}
	}
	return 0;
}
