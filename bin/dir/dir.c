#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[], char* envp[]) {
  int i=1;
printf("Executing dir.c");

//char *filename = "bin/hello3";
 char *dirname = "bin/";
 
 void *dirstr= opendir(dirname);
 
 struct fdMetadata *metadata = (struct fdMetadata*) dirstr ;
  
 printf("metadata->fileDescriptor=%d,size=%d,offset=%d",metadata->fileDescriptor,metadata->size,metadata->offset);
 
 struct dirent* direntptr;
  int ind;
  
  for(ind=0;;ind++){
    direntptr=readdir(dirstr);
    if (direntptr==NULL)
       break;
    else   
       printf("\ndirentptr->d_ino=%d,d_off=%d,d_reclen=%d,d_name=%s",direntptr->d_ino,direntptr->d_off,direntptr->d_reclen,direntptr->d_name);

  }
 
 printf("\n Exited gracefully...");
 
/* 
 int fd=open(filename,0);
 printf("fd returned is %d",fd);
 
 char buf[1000];
 
 ssize_t bytecnt=read(fd,(void *)buf,3);  //Read 3 bytes
  buf[3]='\0';
  printf("Bytes read=%d, Buff contents=%s",bytecnt,buf);
  
  bytecnt=read(fd,(void *)buf,20);  //Read 20 bytes
  buf[20]='\0';
  printf("Bytes read=%d, Buff contents=%s",bytecnt,buf);
  
  int ret=close(fd);
  printf("Return code from close = %d",ret);
 */


/*
 	char *dirname = "mnt/";
  void *dirstr=    opendir(dirname);
  
  
  struct dirent* direntptr;
  int ind;
  
  for(ind=0;ind<=5;ind++){
    direntptr=readdir(dirstr);
    printf("direntptr->d_ino=%d,d_off=%d,d_reclen=%d,d_name=%s",direntptr->d_ino,direntptr->d_off,direntptr->d_reclen,direntptr->d_name);
    if (direntptr==NULL)
       break;
  }     

*/


while(1){
 i++;
 i--;
}
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
