#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[], char* envp[]) {
  argc=1;

//printf("Entered pipe.c with argc=%d",argc);

int fd[2];
int ret=pipe(fd);

//printf("return code from pipe=%d",ret);

//printf("fd[0]=%d, fd[1]=%d",fd[0],fd[1]);

char buf[]="Seawolves";
ret=write(fd[1],(void *)buf,strlen(buf)+1);
//printf("Ret from write=%d",ret);

char buf2[50];
ret=read(fd[0],(void *)buf2,50);
//printf("Ret from read=%d",ret);
//printf("Contents of buf2=%s",buf2);


dup2(1,5);   //copy fd 1 to fd 5. ("park" stdout in fd 5)

dup2(fd[1],1); //fd 1 now points to the write end of the pipe

//printf(" test message to pipe "); //this should now be written to the pipe instead of the console

dup2(5,1); //fd 1 now points to stdout
//printf(" Back in business...");

//check if pipe contains the previous printf contents

ret=read(fd[0],(void *)buf2,50);
printf(" Ret from read=%d ",ret);
//printf(" Contents of buf2=%s ",buf2);




	return 0;
}
