#include<stdio.h>
int main(){
	while(1){
	}
	return 0;
}
//To find out priority of a running process
//	ps -l pid (Check ni-nice value)
//To change its priority use
//	renice newPriority -p pid
//Now to check again if the priority has changed
//	ps -l pid (check ni-nice value)
