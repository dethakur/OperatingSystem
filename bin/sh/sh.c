#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[], char* envp[]) {
	pid_t pid = fork();
	if(pid != 0){
		int status;
		waitpid(pid,&status,0);
	}else{
		printf("%d\n",argc);
		printf("%s\n",argv[0]);
		printf("%s\n",argv[1]);
		execve("sbush",argv,envp);
	}
}
