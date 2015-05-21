#ifndef _SYSCALL_H
#define _SYSCALL_H

#include <sys/defs.h>
#include <stdio.h>
#include <sys/syscall.h>


static __inline uint64_t syscall_0(uint64_t n) {
	uint64_t output;
//	printf("%d",n);
	__asm__ volatile("movq %1, %%rax;"
	    "int $0x80;"
	    "movq %%rax, %0;"
	     :"=g"(output)
	     :"g"(n)
	     :"rax", "rbx","rdi","rdx","rcx","rsi","rbp","r8","r9","r10","r11","r12","r13","r14","r15"
            );
//	printf("%d",n);
	return output;
}

static __inline uint64_t syscall_1(uint64_t n, uint64_t a1) {
	uint64_t output;
	__asm__ volatile("movq %1, %%rax;"
            "movq %2, %%rdi;"
	    "int $0x80;"
	    "movq %%rax, %0;"
	     :"=g"(output)
	     :"g"(n),"g"(a1)
	     :"rax", "rbx","rdi","rdx","rcx","rsi","rbp","r8","r9","r10","r11","r12","r13","r14","r15"
            );
	  return output;
}

static __inline uint64_t syscall_2(uint64_t n, uint64_t a1, uint64_t a2) {
	uint64_t output;
	__asm__ volatile("movq %1, %%rax;"
	    "movq %2, %%rdi;"
	    "movq %3, %%rsi;"
	    "int $0x80;"
	    "movq %%rax, %0;"
	     :"=g"(output)
	     :"g"(n),"g"(a1),"g"(a2)
	     :"rax", "rbx","rdi","rdx","rcx","rsi","rbp","r8","r9","r10","r11","r12","r13","r14","r15"
            );
	
	return output;
}

static __inline uint64_t syscall_3(uint64_t n, uint64_t a1, uint64_t a2, uint64_t a3) {
	uint64_t output;

	__asm__ volatile("movq %1, %%rax;"
	    "movq %2, %%rdi;"
	    "movq %3, %%rsi;"
	    "movq %4, %%rdx;"
	    "int $0x80;"
	    "movq %%rax, %0;"
	     :"=g"(output)
	     :"g"(n),"g"(a1),"g"(a2),"g"(a3)
	     :"rax", "rbx","rdi","rdx","rcx","rsi","rbp","r8","r9","r10","r11","r12","r13","r14","r15"
            );
	
	return output;
}

#endif
