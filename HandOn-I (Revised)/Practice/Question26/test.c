#include<stdio.h>
int main(int argc, char *args[]){
	printf("Message from Executor : ");
	if(argc == 1){
		printf("%s\n", args[0]);
	}
	printf("Program executed successfully.\n");
	return 0;
}
