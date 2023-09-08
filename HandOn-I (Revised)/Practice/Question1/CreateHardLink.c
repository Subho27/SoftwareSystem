#include<stdio.h>
#include<unistd.h>
int main(int argc, char *args[]){
	char *filepath, *hardlinkpath;
	if(argc != 3){
		printf("Please enter filename and hardlink name.\n");
	}
	else{
		filepath = args[1];
		hardlinkpath = args[2];
		int status = link(filepath, hardlinkpath);
		if(status == -1){
			printf("There was a error creating a hard link.\n");
		}
		else{
			printf("Successfully created hard link.\n");
		}
	}
}
