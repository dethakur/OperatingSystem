#include <stdio.h>

int main(int argc, char* argv[], char* envp[]) {
  int i=1;
//  __asm("cli");
//  
 
//	 __asm("int $0x80");
int j = 5;
  while (1) {
	if(i%19200000 ==0){  
//		j += 3;
		printf("%d",j);
//		printf("\n");
		i=0;
//		j= j>=9?1:j;
	}
   	i++;

  } 
	return 0;
}
