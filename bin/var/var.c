#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[], char* envp[]) {

	printf("Argc = %d\n",argc);
	int i=0;
	for(i=0;i<argc;i++){
		printf("Argv = %s\n",argv[i]);

	}
}
