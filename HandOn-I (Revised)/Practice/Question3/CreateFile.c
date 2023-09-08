#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
int main(int argc, char *args[]){
	char *filepath;
	if(argc != 2){
		printf("Please enter filename to create.\n");
	}
	else{
		filepath = args[1];
		int fd = creat(filepath, 0644);
		if(fd == -1){
			printf("There was a error creating a file.\n");
		}
		else{
			printf("Successfully created file and File Descriptor value = %d.\n",fd);
		}
	}
	return 0;
}
