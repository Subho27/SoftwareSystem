#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
int main(int argc, char *args[]){
	char *filename;
	char b[1];
	if(argc != 2){
		printf("Please enter a file to write.\n");
		return 0;
	}
	else{
		filename = args[1];
		int fd = open(filename, O_CREAT | O_RDWR | O_TRUNC, 0777);
		int wbyte = write(fd, "SOFTWARESY", 10);
		if(wbyte != -1){
			int lseek_res = lseek(fd, 10, SEEK_CUR);
			if(lseek_res == -1){
				printf("There is a problem in lseek.\n");
			}
			else{
				printf("Return value of lseek : %d\n", lseek_res);
			}
		}
		else{
			printf("There is a problem in writing.\n");
		}
		wbyte = write(fd, "STEMSCS513", 10);
		if(wbyte == -1){
			printf("There is a problem in writing again.\n");
		}
		printf("File Content : \n");
		int fd1 = open(filename, O_RDONLY);
		while(1){
			int rbyte = read(fd1, b, 1);
			if(rbyte == 0){
				printf("\nFinished reading.\n");
				break;
			}
			printf("%c",b[0]);
		}
	}
	return 0;
}

//There will be null values in between those two strings
//while printing with printf we will not be able to see it
//Please print with command "od -c filename" to see
