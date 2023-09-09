#include<unistd.h>
#include<stdio.h>
void main()
{
    char *command = "/bin/ls";
    char *options = "-Rl";
    char *file_path = "../Question16";
    char * arr[] = {
    	command,
    	options,
    	file_path,
    	NULL
    };
    printf("Starting program execution...\n");
    int status = execvp(command, arr);
    if(status == -1){
    	printf("Program execution failed.");
    }
}
