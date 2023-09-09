#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
int main(){
	int pid = getpid();
	printf("Parent process id : %d\n", pid);
	int cpid = fork();
	if(cpid > 0){
		printf("Process has gone to sleep for 50 sec.\n");
		printf("Zombie process id will be : %d\n", cpid);
		sleep(50);
		printf("Process has woken up.\n");
	}
	else{
		printf("Child is being shut down.\n");
		printf("Creating zombie.\n");
		exit(0);
	}
	return 0;
}
