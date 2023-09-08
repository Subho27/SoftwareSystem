#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>
#include<stdlib.h>
int main(int argc, char *args[]){
	char *filename;
	int fd;
	struct flock lock, lockStatus;
	filename = "TRSdata.txt";
	
	fd = open(filename, O_CREAT | O_RDWR, 0777);
	
	if(fd == -1){
		printf("There is a problem with opening the file.");
		return 0;
	}
	printf("---Ticket Reservation System---\n");
	printf("1. Read a ticket info.\n");
	printf("2. Write a ticket info.\n");
	printf("3. Exit.\n");

	while(1){
		printf("Please Enter your choice : \n");

	        int choice;
        	scanf("%d", &choice);

		switch(choice){
			case 1: lock.l_type = F_RDLCK;
				fcntl(fd, F_GETLK, &lockStatus);
				if(lockStatus.l_type == F_WRLCK){
					printf("Another process is writing to this file.\nPlease wait for sometime.\n");
				}
				else{
					int status = fcntl(fd, F_SETLK, &lock);
					if(status == -1){
						printf("There is a problem with locking the file.\n");
						break;
					}
					else{
						lseek(fd, 0, SEEK_SET);
						int b;
						int rbyte = read(fd, &b, sizeof(int));
						if(rbyte == 0){
							printf("Your ticket reservation number is not generated yet.\n");
							break;
						}
						printf("Your ticket reservation number is %d\n", b);
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
			case 2: lock.l_type = F_WRLCK;
				fcntl(fd, F_GETLK, &lockStatus);
				if(lockStatus.l_type == F_WRLCK){
					printf("Another process is writing to this file.\nPlease wait for sometime.\n");
				}
				else if(lockStatus.l_type == F_RDLCK){
					printf("Another process is reading from this file.\nPlease wait for sometime.\n");
				}
				else{
					int status = fcntl(fd, F_SETLK, &lock);
					if(status == -1){
						printf("There is a problem with locking the file.\n");
						break;
					}
					else{
						lseek(fd, 0, SEEK_SET);
						int b;
						int rbyte = read(fd, &b, sizeof(int));
						if(rbyte == 0){
							b = 1;
						}
						else{
							b++;
						}
						lseek(fd, 0, SEEK_SET);
						int wbyte = write(fd, &b, sizeof(int));
						printf("Your ticket updated reservation number is %d\n", b);
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
			case 3: printf("Exiting...");
				return 0;				
		}
	}
	return 0;
}
