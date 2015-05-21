#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[], char* envp[]) {
	printf("%d\n",argc);
	printf("old %s\n",argv[0]);
//	strcpy("sbush",argv[0]);
	printf("new %s\n",argv[0]);
	printf("arg = %s\n",argv[1]);
	execve("sbush",argv,envp);
/*	pid_t pid = fork();
	if(pid != 0){
		int status;
		waitpid(pid,&status,0);
	}else{
	}*/
}
