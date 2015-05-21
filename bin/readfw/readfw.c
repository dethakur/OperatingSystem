#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[], char* envp[]) {
char *inputVar = (char*)malloc(200*sizeof(char));

read(0,inputVar,200);
*(inputVar+10)='\0';
printf("\n first 10 characters of input = %s",inputVar);

	return 0;
}
