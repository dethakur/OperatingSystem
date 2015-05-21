#include <stdio.h>
#include <stdlib.h>
#define BYTES_PER_READ 100

int main(int argc, char* argv[], char* envp[]) {

	//printf("Entered cat.c");

	char buf[NAME_MAX];
	int bytes_read=0;
	getcwd(buf,NAME_MAX);
	//printf("Curr working dir=%s |",buf);



	char filename[NAME_MAX];

	if (argc==1){   
		char *inputVar = (char*)malloc(200*sizeof(char));
		bytes_read=read(0,inputVar,200);
		//  printf("\n bytes_read from user=%d, user entry=%s",bytes_read,inputVar);
		char *ptr=inputVar;
		//if there are newlines,null-terminate the user input
		while(ptr){
			if (*ptr=='\n'){
				*ptr='\0';
				break;
			}
			ptr++;
		}    
		//    *(inputVar+bytes_read)='\0';
		if(*inputVar=='/')             //user supplied an absolute path
			strcpy(inputVar+1,filename);
		else                          //user supplied a relative path
			strconcat(buf+1,inputVar,filename);  //append the path supplied by the user to the cwd
	}else{
		if(*argv[1]=='/')             //user supplied an absolute path
			strcpy(argv[1]+1,filename);
		else                          //user supplied a relative path
			strconcat(buf+1,argv[1],filename);  //append the path supplied by the user to the cwd
	}       


	//printf("\nfilename passed to opendir=%s",filename);

	int fd=open(filename,0);
	//printf("\nfd returned is %d",fd);

	printf("\n");
	while(1){
		bytes_read=read(fd,(void *)buf,BYTES_PER_READ);
		if(bytes_read==0)
			break;
		else if(bytes_read<0){
			printf("\n Error encountered during read");
			break;   
		}else{
			*(buf+bytes_read)='\0'; 
			printf("%s",buf);
		}    
	}
	/*int ret=*/close(fd);
	//printf("Return code from close = %d",ret);


	return 0;
}
























/*  //old code
    int main(int argc, char* argv[], char* envp[]) {
    int i=1;
//  __asm("cli");
//  

//	 __asm("int $0x80");
int j = 1;
while (1) {
if(i%19200000 ==0){  
//		j += 1;
printf("%d",j);
//	printf("\n");
i=0;
//		j= j>=9?1:j;
}
i++;

} 
return 0;
}

*/
