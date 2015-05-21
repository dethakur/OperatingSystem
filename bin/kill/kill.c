#include <stdio.h>
#include <stdlib.h>
#include <syscall.h>
#include <sys/defs.h>

extern int stoi(char*);
int main(int argc, char* argv[], char* envp[]) {
	int pid = stoi(argv[2]);
	printf("[kill.c] PID received %d\n",pid);
	if(pid == 1){
		printf("You cannot kill Init process. Access denied \n");
		return 0;
	}
	syscall_1(SYS_kill,pid);
	return 0;
}
