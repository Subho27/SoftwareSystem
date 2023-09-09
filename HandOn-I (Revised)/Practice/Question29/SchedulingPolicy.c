#include <sched.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

void main()
{
    int currP, pid;
    pid = getpid();
    currP = sched_getscheduler(pid);
    struct sched_param priority;
    
    priority.sched_priority = 10;

    switch (currP)
    {
    case SCHED_FIFO:
        printf("Current policy is FIFO\n");
        sched_setscheduler(pid, SCHED_RR, &priority);
        currP = sched_getscheduler(pid);
        if(currP == SCHED_RR) printf("Current policy is SCHED_RR\n");
        else printf("Could not set policy, Please use \"sudo\"\n");
        break;
    case SCHED_RR:
        printf("Current policy is RR\n");
        sched_setscheduler(pid, SCHED_FIFO, &priority);
        currP = sched_getscheduler(pid);
        if(currP == SCHED_FIFO) printf("Current policy is SCHED_FIFO\n");
        else printf("Could not set policy, Please use \"sudo\"\n");
        break;
    case SCHED_OTHER:
        printf("Current policy is OTHER\n");
        sched_setscheduler(pid, SCHED_RR, &priority);
        currP = sched_getscheduler(pid);
        if(currP == SCHED_RR) printf("Current policy is SCHED_RR\n");
        else printf("Could not set policy, Please use \"sudo\"\n");
        break;
    default:
        perror("Error while getting current policy\n");
    }
}
