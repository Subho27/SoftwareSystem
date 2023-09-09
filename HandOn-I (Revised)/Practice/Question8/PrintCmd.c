#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
int main(int argc, char *args[]){
	char *from;
	char b[1];
	char data[1024];
	int rbyte, wbyte;
       	int count = 0;
	if(argc != 2){
		printf("Please enter files to copy from");
	}
	else{
		from = args[1];
		int fromfd = open(from, O_RDONLY);
		while(1){
			int flag = 0;
			while(1){
				rbyte = read(fromfd, b, 1);
				if(rbyte == 0){
					flag = 1;
					break;
				}
				else if(rbyte == -1){
					printf("There is an error in reading file.\n");
					break;
				}
				else if(b[0] == '\n'){
					printf("\nLine read completed.\n");
					break;
				}
				else{
					data[count] = b[0];
					count++;
				}
			}
			if(flag == 1){
				printf("Exiting...\n");
				break;
			}
			for(int i=0; i<count; i++){
				putchar(data[i]);
			}
			printf("\n");
			count = 0;
		}
	}
	return 0;
}
