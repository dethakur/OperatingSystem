#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[], char* envp[]) {
	int i=0;
	if(argc >1){
		for(i=1;i<argc;i++){
			printf("%s\n",argv[i]);
		}
	}

}
