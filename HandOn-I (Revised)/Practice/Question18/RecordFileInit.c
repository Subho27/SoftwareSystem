#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
int main(){
	int fd;
	struct flock lock, lockStatus;
	char *filename;

	filename = "Recordfile.txt";

	fd = open(filename, O_CREAT | O_TRUNC | O_RDWR, 0777);
	
	if(fd == -1){
		printf("Record file is already there, No need to create.\n");
		return 0;
	}

	lock.l_type = F_WRLCK;

	fcntl(fd, F_GETLK, &lockStatus);
	if(lockStatus.l_type == F_WRLCK){
		printf("Already file is locked for writing by other file.\n");
		return 0;
	}
	else{
		int status = fcntl(fd, F_SETLK, &lock);
		if(status == -1){
			printf("Not able to acquire write lock. Hence terminating.\n");
			return 0;
		}
		char arr = '\n';
	       	for(int i=0; i<3; i++){
			int rec = i+1;
			lseek(fd, 0, SEEK_END);
			write(fd, &rec, sizeof(int));
			write(fd, &rec, sizeof(int));
			write(fd, &arr, sizeof(char));
		}	
		printf("Completed record initialization.\n");
		lock.l_type = F_UNLCK;
		int st = fcntl(fd, F_SETLK, &lock);
		if(st == -1){
			printf("Could not unlock the file.\n");
		}
		else{
			printf("Successfully unlocked the file.\n");
		}
	}

	return 0;
}
