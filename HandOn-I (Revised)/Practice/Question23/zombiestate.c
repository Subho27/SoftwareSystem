#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
int main(){
	int pid = getpid();
	printf("Parent process id : %d", pid);
	int cpid = fork();
	if(cpid > 0){
		printf("Process has gone to sleep for 20 sec.");
		sleep(50);
		printf("Process has woken up.");
	}
	else{
		printf("Child is being shut down.");
		printf("Creating zombie.");
		exit(0);
	}
	return 0;
}
