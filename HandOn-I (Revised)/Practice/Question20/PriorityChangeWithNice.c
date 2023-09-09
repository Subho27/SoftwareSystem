#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

void main()
{
    int priority, newp;
    priority = nice(0);
    printf("Current priority: %d\n", priority);
    printf("Enter new priority you want to assign : ");
    scanf("%d", &newp);
    priority = nice(newp);
    printf("Current priority: %d\n", priority);
}
