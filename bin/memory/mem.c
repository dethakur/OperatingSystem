#include <stdio.h>
#include <stdlib.h>
#include <syscall.h>

int main(int argc, char* argv[], char* envp[]) {
	//iint i=1;

	while(1){
		char* a = malloc(1000);	
		read(0,a,1000);
		printf("Entered%s\n",a);
	}

	return 0;
}
