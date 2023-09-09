#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include<sys/resource.h>
void main()
{
    int priority, newp, pid;
    pid = getpid();
    priority = getpriority(PRIO_PROCESS, pid);
    printf("process id: %d\n", pid);
    printf("Current priority: %d\n", priority);
    printf("Enter new priority you want to assign : ");
    scanf("%d", &newp);
    int status = setpriority(PRIO_PROCESS, pid, newp);
    if(status == -1){
    	printf("There was an error in setting priority");
    }
    else{
    	priority = getpriority(PRIO_PROCESS, pid);
	printf("Current priority: %d\n", priority);
    }
}
