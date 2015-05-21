#include<sys/sbunix.h>
#include<sys/defs.h>
#include<stdlib.h>

#define U_ENVP 0xFFFFF000

void _start(void) {
	uint64_t *x = (uint64_t*)U_ENVP; 
	int argc =*x; 
	x++;
	x++;

	char* ptr = (char*)x;
	char* argv[10];
	for(int i=0;i<argc;i++){
		argv[i] = ptr;
		//while(*ptr != '\0')
		//	ptr++;
		ptr = ptr+strlen(ptr);
		ptr++;
	}
	
	ptr++;

	char* envp[10];
	int j = 0;
	while(*ptr != 0){
		envp[j] = ptr;
		//while(*ptr != '\0')
		//	ptr++;
		ptr = ptr + strlen(ptr);
		ptr++;
		j++;
	}
	int res = 100;
	res = main(argc, argv, envp);
	exit(res);
}
void _start_t(void) {
//	uint64_t *x = (uint64_t*)U_ENVP; 
//	int argc =(int)(*x); 
//	int res = 100;
//	x = x + 1;//sizeof(char);
//	char** newArgv = (char**)(x);
//	char** newEnvp = newArgv + argc+1;
	int argc = 1;	
	char* argv[10];
	char* envp[10];
	main(argc, argv,envp);
	exit(0);
//	print("Using %d",res);
}

