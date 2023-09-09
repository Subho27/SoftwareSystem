#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>
#include<stdlib.h>
int main(int argc, char *args[]){
	char *filename;
	int fd;
	struct flock lock, lockStatus;
	filename = "Recordfile.txt";
	
	fd = open(filename, O_RDWR);
	
	if(fd == -1){
		printf("There is a problem with opening the file.");
		return 0;
	}
	printf("---Record Modifier System---\n");
	printf("1. Read a record info.\n");
	printf("2. Write a record info.\n");
	printf("3. Exit.\n");

	while(1){
		printf("Please Enter your choice : \n");

	        int choice;
        	scanf("%d", &choice);

		switch(choice){
			case 1: printf("Please enter which record you want to read(1 to 3)?\n");
				int no;
				scanf("%d", &no);
				int bytetoSkip = (no-1) * ((sizeof(int))*2+ sizeof(char));
				lockStatus.l_type = F_RDLCK;
				lockStatus.l_whence = SEEK_SET;
				lockStatus.l_start = bytetoSkip;
				lockStatus.l_len = 2 * sizeof(int);
				fcntl(fd, F_GETLK, &lockStatus);
				if(lockStatus.l_type == F_WRLCK){
					printf("Another process is writing to this file.\nPlease wait for sometime.\n");
				}
				else{
					lockStatus.l_type = F_WRLCK;
					int status = fcntl(fd, F_SETLK, &lockStatus);
					if(status == -1){
						printf("There is a problem with locking the file.\n");
						break;
					}
					else{
                                		lseek(fd, bytetoSkip, SEEK_SET);
						int b1;
						int b2;
						int rbyte = read(fd, &b1, sizeof(int));
						rbyte = read(fd, &b2, sizeof(int));
						if(rbyte == 0){
							printf("Record number is not generated yet.\n");
							break;
						}
						printf("||||||Record number %d contains : %d||||||\n", b1, b2);
						lock.l_type = F_UNLCK;
						int status = fcntl(fd, F_SETLK, &lock);
						if(status == -1){
							printf("There is a problem with unlocking the file.\n");
						}else{
							printf("File is unlocked for reading and writing.\n");
						}
					}
				}
				break;
			case 2: printf("Please enter which record you want to write(1 to 3)?\n");
                                int num, data;
                                scanf("%d", &num);
				printf("Please enter the new data : ");
				scanf("%d", &data);
				int byteto = ((num-1) * ((sizeof(int))*2+ sizeof(char))) + sizeof(int);
				lockStatus.l_type = F_WRLCK;
				lockStatus.l_whence = SEEK_SET;
                                lockStatus.l_start = byteto;
                                lockStatus.l_len = sizeof(int);
				fcntl(fd, F_GETLK, &lockStatus);
				if(lockStatus.l_type == F_WRLCK){
					printf("Another process is writing to this file.\nPlease wait for sometime.\n");
				}
				else if(lockStatus.l_type == F_RDLCK){
					printf("Another process is reading from this file.\nPlease wait for sometime.\n");
				}
				else{
					lockStatus.l_type = F_WRLCK;
					int status = fcntl(fd, F_SETLK, &lockStatus);
					if(status == -1){
						printf("There is a problem with locking the file.\n");
						break;
					}
					else{
						//To test locking uncomment these lines
						int n;
						scanf("%d", &n);
                                                lseek(fd, byteto, SEEK_SET);
						int wbyte = write(fd, &data, sizeof(int));
						lseek(fd, -(2*sizeof(int)), SEEK_CUR);
						int b1;
                                                int b2;
                                                int rbyte = read(fd, &b1, sizeof(int));
                                                rbyte = read(fd, &b2, sizeof(int));
                                                if(rbyte == 0){
                                                        printf("Record number is not generated yet.\n");
                                                        break;
                                                }
                                                printf("||||||Record number %d contains : %d||||||\n", b1, b2);
						lock.l_type = F_UNLCK;
						int status = fcntl(fd, F_SETLK, &lock);
						if(status == -1){
							printf("There is a problem with unlocking the file.\n");
						}else{
							printf("File is unlocked for reading and writing.\n");
						}
					}
				}
				break;
			case 3: printf("Exiting...\n");
				return 0;				
		}
	}
	return 0;
}
