#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
int main(){
	int pid = getpid();
	printf("Parent process id : %d\n", pid);
	int cpid = fork();
	if(cpid > 0){
		printf("Process has gone to sleep for 10 sec.\n");
		printf("Orphan process id will be : %d.\n", cpid);
		sleep(5);
		printf("Process has woken up and exiting..\n");
		exit(0);
	}
	else{
		printf("Child is going to sleep for 50 sec.\n");
		sleep(15);
		printf("Child has woken up.\nOrphan process is created.\n");
		int ppidp = getppid();
		printf("Parent process id after creating orphan : %d.\n", ppidp);
	}
	return 0;
}
