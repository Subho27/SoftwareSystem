#include <sched.h>
#include <stdio.h> 

void main()
{
    int maxP, minP;

    maxP= sched_get_priority_max(SCHED_RR);
    if (maxP == -1)
        perror("Error while finding max priority\n");
    else
        printf("The max priority with RR Scheduling Policy is : %d\n", maxP);

    minP = sched_get_priority_min(SCHED_RR);
    if (minP == -1)
        perror("Error while finding min priority\n");
    else
        printf("The min priority with RR Scheduling Policy is : %d\n", minP);
}
