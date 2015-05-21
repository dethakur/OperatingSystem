#include <stdio.h>
#include <stdlib.h>

void func1();
int main(int argc, char* argv[], char* envp[]) {
	while(1){
		char a[4000];
		a[10]='x';
		func1();
		printf("\n%c\n",a[10]);
		sleep(3);
	}

}
void func1(){
	char a[4000];
	a[100]='f';
	printf("\n%c\n",a[100]);
	func1();
}
