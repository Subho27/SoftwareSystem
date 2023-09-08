#include<stdio.h>
#include<unistd.h>
#include<sys/stat.h>
//#include<sys/types.h>
int main(int argc, char *args[]){
	char* mkfifopath = "myfifo-mkfifo";
        char* mknodepath = "myfifo-mknode";
        int status1 = mkfifo(mkfifopath, 0777);
        if(status1 == -1){
                printf("There was a error creating a Fifo using mkfifo.\n");
        }
        else{
                printf("Successfully created Fifo using mkfifo.\n");
        }

	int status2 = mknod(mknodepath, S_IFIFO | 0777, 0);
	if(status1 == -1){
                printf("There was a error creating a Fifo using mknode.\n");
        }
        else{
                printf("Successfully created Fifo using mknode.\n");
        }
	return 0;
}
