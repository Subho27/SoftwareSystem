#include<stdio.h>
#include<unistd.h>
int main(){
	int pid = getpid();
	printf("Parent process id : %d.\n",pid);
	int cpid = fork();
	printf("Child process id : %d\n",cpid);
	return 0;
}
