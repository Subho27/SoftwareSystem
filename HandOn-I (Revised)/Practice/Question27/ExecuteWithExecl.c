#include<unistd.h>
#include<stdio.h>
void main()
{
    char *command = "/bin/ls";
    char *options = "-Rl";
    char *file_path = "../Question16";
    
    printf("Starting program execution...\n");
    int status = execl(command, command, options, file_path, NULL);
    if(status == -1){
    	printf("Program execution failed.");
    }
}
