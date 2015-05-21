#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[], char* envp[]) {
//char *arg[3];
//arg[0]=(char *)malloc(10);
//arg[1]=(char *)malloc(10);
//arg[2]=(char *)malloc(10);

//char *tempstr=malloc(10);
//globenvp=envp;
//char **arg=(char**)malloc(30*sizeof(char*));

	//  int i=1;
	//  __asm("cli");
	//  

	//	 __asm("int $0x80");
	//int j = 1;
	printf("Hello World\n");
 
	pid_t id = fork();
	if(id != 0){  //parent
		printf("[Exit] Waiting for process id = %d\n",id);
		int status;
		waitpid(id,&status,0);
		printf("Ready for next fork \n");
	}else{  //child
	//set up args
     printf("\n Entered child");
     
     char *arg[] = {"bin/ls", "/mnt",NULL};
     //strcpy("bin/ls",arg[0]);
     //strcpy("/mnt",arg[1]);
     //arg[2]=NULL;
     
     execve(arg[0],arg,envp);
		printf("SOme how it came here");
	}
 
 //-----------------------------------------------


 	id = fork();
	if(id != 0){  //parent
		printf("[Parent] Waiting for process id = %d\n",id);
		int status;
		waitpid(id,&status,0);
		printf("[Parent       ] Parent will exit now \n");
	}else{  //child
	//set up args
     printf("\n Entered child");
     char *arg[] = {"bin/ls",NULL};
     execve(arg[0],arg,envp);
		printf("SOme how it came here");
	}
 

 
	return 0;
}



