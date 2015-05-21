#include <stdio.h>

int main(int argc, char* argv[], char* envp[]) {
//while(1);	
  int i=1;
//  __asm("cli");
//  
 
//	 __asm("int $0x80");
int j = 8;
  while (1) {
	if(i%19200000 ==0){  
		j += 1;
//		printf("%d",j);
	//	printf("\n");
		i=0;
		j= j>=9?1:j;
	}
   	i++;

  } 
	return 0;
}
