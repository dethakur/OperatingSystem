#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[], char* envp[]) {
	fork();
	fork();
	fork();
	fork();
	fork();
	fork();
//	fork();
	execve("bin/ls",argv,envp);


}
