#include<stdio.h>
#include<unistd.h>
int main(int argc, char *args[]){
	char* b[100];
	int rfd, wfd;
	while(1){
		rfd = read(0, b, 1);	//STDIN - 0
		wfd = write(1, b, 1);	//STDOUT - 1
		if(rfd == -1 || wfd == -1){
			printf("Fatal error: while reading or writing.\n");
			break;
		}
	}
	return 0;
}
