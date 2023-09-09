#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/wait.h>
int main(){
	int pid, child1, child2, child3;
	pid = getpid();
	printf("Parent process id : %d\n", pid);
	child1 = fork();
	if(child1 > 0){
		child2 = fork();
		if(child2 > 0){
			child3 = fork();
			if(child3 > 0){
				int waitStatus;
				int terminatedChild = waitpid(child3, &waitStatus, 0);

				if (terminatedChild == child3)
				{
				    if (waitStatus == 0)
				        printf("Child 3 has terminated parent with exit status = 0\n");
				    else
				        printf("Child has exited with exit status : %d\n", waitStatus);
				}
				
			}
			else{
				sleep(2);
				printf("Child 3 is created.\n");
				printf("Child 3 process id : %d.\n", getpid());
				printf("Child 3 is going to sleep for 15 sec.\n");
				sleep(15);
				printf("Child 3 is awake now.\n");
			}
		}
		else{
			sleep(1);
			printf("Child 2 is created.\n");
			printf("Child 2 process id : %d.\n", getpid());
			printf("Child 2 is going to sleep for 10 sec.\n");
			sleep(10);
			printf("Child 2 is awake now.\n");
		}
	}
	else{
		printf("Child 1 is created.\n");
		printf("Child 1 process id : %d.\n", getpid());
		printf("Child 1 is going to sleep for 5 sec.\n");
		sleep(5);
		printf("Child 1 is awake now.\n");
	}
	return 0;
}
