#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
int main(int argc, char *args[]){
	char *filename;
	if(argc != 2){
		printf("Please enter a filename to check.");
	}
	else{
		filename = args[1];
		int fd = open(filename, O_RDWR);
		int status = fcntl(fd, F_GETFL);
		if (status == -1){
                	printf("Error on executing fcntl!");
		}
           	 else
            	{
                	switch (O_ACCMODE & status)
                	{
                		case 0:
                    			printf("The file has been opened with the flags : O_RDONLY\n");
                    			break;
                		case 1:
                    			printf("The file has been opened with the flags : O_WRONLY\n");
                    			break;
                		case 2:
                    			printf("The file has been opened with the flags : O_RDWR\n");
                    			break;
               		        case 64:
                    			printf("The file has been opened with the flags : O_CREAT\n");
                    			break;
                		case 512:
                    			printf("The file has been opened with the flags : O_TRUNC\n");
                    			break;
                		case 1024:
                    			printf("The file has been opened with the flags : O_APPEND\n");
                    			break;
                		case 577:
                			printf("The file has been opened with the flags : O_WRONLY | O_CREAT | O_TRUNC\n");
                   		 	break;
                		case 1089:
                    			printf("The file has been opened with the flags : O_WRONLY | O_CREAT | O_APPEND\n");
                    			break;
                		case 578:
                    			printf("The file has been opened with the flags : O_RDWR | O_CREAT | O_TRUNC\n");
                    			break;
                		case 1090:
                    			printf("The file has been opened with the flags : O_RDWR | O_CREAT | O_APPEND\n");
                    			break;
                		default:
                    			printf("Error!");
                	}
            	}
	}
	return 0;
}
