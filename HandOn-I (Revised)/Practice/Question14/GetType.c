#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/stat.h>
int main(int argc, char *args[]){
	char *filename;
	struct stat buffer;
	if(argc != 2){
		printf("Please enter a filename to check.");
	}
	else{
		filename = args[1];
		int fd = open(filename, O_RDWR);
		int status = fstat(fd, &buffer);
		if (status == -1){
                	printf("Could not get status!\n");
		}
           	 else
            	{
                	if(S_ISDIR(buffer.st_mode)){
				printf("Entered file is of type : Directory.\n");
			}
			else if(S_ISCHR(buffer.st_mode)){
				printf("Entered file is of type : Character Special file.\n");
			}
			else if(S_ISBLK(buffer.st_mode)){
				printf("Entered file is of type : Block Special file.\n");
			}
			else if(S_ISREG(buffer.st_mode)){
				printf("Entered file is of type : Regular file.\n");
			}
			else if(S_ISFIFO(buffer.st_mode)){
				printf("Entered file is of type : FIFO Special file.\n");
			}
			else if(S_ISLNK(buffer.st_mode)){
				printf("Entered file is of type : Symbolic Link.\n");
			}
			else if(S_ISSOCK(buffer.st_mode)){
				printf("Entered file is of type : Socket.\n");
			}
			else{
				printf("Sorry! could not recognise file type.\n");
			}
            	}
	}
	return 0;
}
