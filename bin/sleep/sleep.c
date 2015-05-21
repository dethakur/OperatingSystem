#include <stdio.h>
#include <stdlib.h>
extern int stoi(char*);
int main(int argc, char* argv[], char* envp[]) {
	if(argc>1){
		int time = stoi(argv[1]); 
		printf("Sleeping for %d seconds\n",time);
		sleep(time);	
	}else{
		printf("Called sleep without any parameter. Will prempt\n");
	}
}
