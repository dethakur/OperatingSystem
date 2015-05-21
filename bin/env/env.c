#include <stdio.h>
#include <stdlib.h>
#include <syscall.h>

int main(int argc, char* argv[], char* envp[]) {
	//iint i=1;

	//char *a = malloc(1000);:ta	
	
	char* mypath=(char*)malloc(700*sizeof(char));
	int j=0;
	int i=0;

	for(i=0;envp[i]!=NULL || i <20;i++){
		if ( (*envp[i]=='P') && ( *(envp[i]+1)=='A' ) ){
			for(j=0;*(envp[i]+5+j)!='\0';j++){
				mypath[j]=*(envp[i]+5+j);
			}  
			mypath[j]='\0';
		}
	}

	printf("%s",mypath);
	//  __asm("cli");
	//  

	//	 __asm("int $0x80");
	//int j = 6;
	//char* s = malloc(1000);
	//malloc(1000);

	//brk((uint64_t*)1);
	//syscall_1(12,(uint64_t)1000);
	//brk(0);
	/*printf("1");
	  pid_t t = fork();
	  printf("~~~fork response %d~~",t);
	  return 0;
	  */
	return 0;
}
