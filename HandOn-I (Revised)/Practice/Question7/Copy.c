#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
int main(int argc, char *args[]){
	char *from, *to;
	char* b[100];
	int rbyte, wbyte;
	if(argc != 3){
		printf("Please enter files to copy from & to.");
	}
	else{
		from = args[1];
		to = args[2];
		int fromfd = open(from, O_RDONLY);
		int tofd = open(to, O_WRONLY);
		while(1){
			rbyte = read(fromfd, b, 1);
			wbyte = write(tofd, b, 1);
			if(rbyte == 0){
				printf("Completed copying content.\n");
				break;
			}
		}
	}
	return 0;
}
