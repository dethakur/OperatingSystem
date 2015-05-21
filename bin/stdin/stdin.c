#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[], char* envp[]) {
	char* a = malloc(1000);
	pid_t pid = fork();
	if(pid != 0){
		read(0,a,1000);
		printf("[Parent]Entered = %s",a);
	}else{
		sleep(0);
		read(0,a,1000);
		printf("Comes here. It should be killed\n");
		printf("[Child]Entered = %s",a);
	}


}
