#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[], char* envp[]) {
	//  int i=1;
	//  __asm("cli");
	//  

	//	 __asm("int $0x80");
	//int j = 1;
//	char *a = malloc(1);
	char* a = malloc(1);

	a[6000]= 'a';
	char c = a[1000];
	printf("%c",c);
}
