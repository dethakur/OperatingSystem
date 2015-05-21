#include <stdio.h>
#include <stdlib.h>
#include <syscall.h>

int main(int argc, char* argv[], char* envp[]) {

	pid_t pid = fork();
	if(pid != 0){
		int status;
		waitpid(pid, &status,0);
	}else{
	fork();
	fork();
	fork();
	fork();
	fork();
	fork();
	fork();
	fork();
	fork();
	fork();
	fork();
	}
//	fork();
//	fork();
//	fork();
	

return 0;
}
