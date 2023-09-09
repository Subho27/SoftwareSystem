#include<unistd.h>
#include<stdio.h>
void main()
{
    char *epath = "./test";
    char *arg = "HiIAmExecutor";
    
    printf("Starting program execution...\n");
    int status = execl(epath, arg, NULL);
    if(status == -1){
    	printf("Program execution failed.");
    }
}
