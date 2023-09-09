#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/select.h>
int main(){
	fd_set read_fds, write_fds;
	struct timeval time;
	int returnVal;

	FD_SET(STDIN_FILENO, &read_fds);
	FD_SET(STDOUT_FILENO, &write_fds);

	time.tv_sec = 10;
	time.tv_usec = 0;

	returnVal = select(1, &read_fds, &write_fds, NULL, &time);

	if(returnVal == -1){
		printf("There is an error!\n");
	}
	else if(returnVal){
		printf("The data is now available %d.\n", returnVal);
	}
	else{
		printf("No data was entered.\n");
	}

	return 0;
}
