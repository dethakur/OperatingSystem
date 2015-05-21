#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[], char* envp[]) {

printf("Entered pwd.c");

 char buf[1000];
 getcwd(buf,1000);
 printf("Curr working dir=%s |",buf);
 
 }