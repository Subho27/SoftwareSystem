#include<stdio.h>
#include<unistd.h>
int main(int argc, char *args[]){
	char *filepath, *softlinkpath;
	if(argc != 3){
		printf("Please enter filename and softlink name.\n");
	}
	else{
		filepath = args[1];
		softlinkpath = args[2];
		int status = symlink(filepath, softlinkpath);
		if(status == -1){
			printf("There was a error creating a soft link.\n");
		}
		else{
			printf("Successfully created soft link.\n");
		}
	}
}
