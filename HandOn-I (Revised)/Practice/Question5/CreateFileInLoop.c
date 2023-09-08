#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>
int main(int argc, char *args[]){
	int i = 0;
	while(1){
		if(i >= 5){
			i = i % 5;
		}
		
		char filename[10] = "file";
		char no[10];

		sprintf(no, "%d", i);

		strcat(filename, no);

		int fd = open(filename, O_CREAT | O_RDONLY, 0644);
		if(fd == -1){
			printf("There was an error.");
			return 0;
		}
		else {
			//printf("Successfully created file%d..\n",i);
			//fflush(stdout);
		}
		sleep(3);
		i++;
	}
	return 0;
}

//Run this process in background using & at the end of command
//It will return process id
//check in proc directory, in that pid folder check fd folder
//Continuously opening files
