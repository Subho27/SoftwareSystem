#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
int main(int argc, char *args[]){
	char *filepath;
	if(argc != 2){
		printf("Please enter filename to open.\n");
	}
	else{
		filepath = args[1];
		int fd = open(filepath, O_CREAT | O_EXCL | O_RDWR, 0644);

		if(fd == -1){
			printf("There was a error opening a file.\nIt May fail beacuse of O_EXCL flag.\n");
			return 0;
		}
		else{
			printf("Successfully opened file and File Descriptor value = %d.\n",fd);
		}

		int close_fd = close(fd);
		if(close_fd == -1){
                        printf("Warning : File not yet closed.");
                }
	}
	return 0;
}
