#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[], char* envp[]) {
//  __asm("cli");
//  
 
//	 __asm("int $0x80");
int j = 2;
  while (1) {
//		j += 3;
		sleep(3);
		printf("%d",j);
//		printf("\n");
//		j= j>=9?1:j;
  } 
	return 0;
}
