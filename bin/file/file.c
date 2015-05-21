#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[], char* envp[]) {
 
printf("Entered file.c");

 char buf[1000];
 getcwd(buf,1000);
 printf("Curr working dir=%s |",buf);
 
 int ret=chdir("mnt/");
 getcwd(buf,1000);
 printf("[1]ret = %d, New working dir=%s |",ret,buf);
 
/*
 ret=chdir("/mnt/");
 getcwd(buf,1000);
 printf("[2]ret = %d, New working dir=%s |",ret,buf);
 
 ret=chdir("/rootfs/lib/");
 getcwd(buf,1000);
 printf("[3]ret = %d, New working dir=%s |",ret,buf);
*/ 

//char *filename = "bin/hello3";
 char *filename = "mnt/sample2.txt";
 int fd=open(filename,0);
 printf("fd returned is %d",fd);
 

 
 ssize_t bytecnt=read(fd,(void *)buf,3);  //Read 3 bytes
  buf[3]='\0';
  printf("\nBytes read=%d, Buff contents=%s",bytecnt,buf);
  
  bytecnt=read(fd,(void *)buf,20);  //Read 20 bytes
  buf[20]='\0';
  printf("\nBytes read=%d, Buff contents=%s",bytecnt,buf);
  
  off_t delta=25;
  off_t newoffset=lseek(fd,delta,SEEK_SET);
  printf("\n [USER][SEEK_SET] newoffset returned is %d",newoffset);
  
  delta=4;
  newoffset=lseek(fd,delta,SEEK_CUR);
  printf("\n [USER][SEEK_CUR] newoffset returned is %d",newoffset);
  
  delta=8;
  newoffset=lseek(fd,delta,SEEK_END);
  printf("\n [USER][SEEK_END] newoffset returned is %d",newoffset);
  
  
   
  ret=close(fd);
  printf("Return code from close = %d",ret);



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
