#include <stdio.h>
#include <stdlib.h>
#include <syscall.h>
#include <sys/defs.h>

int main(int argc, char* argv[], char* envp[]) {
	syscall_0(SYS_ps);
}
