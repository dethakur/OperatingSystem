#include<sys/defs.h>

#ifndef ISR_H 
#define ISR_H

#define NAME_MAX 255
//These are for isr routines that have error code pushed onto the stack
//Like GPL , Page Fault etc
struct isr_reg_ec_handlers{
	uint64_t flags;
	uint64_t r15;
	uint64_t r14;
	uint64_t r13;
	uint64_t r12;
	uint64_t r11;
	uint64_t r10;
	uint64_t r9;
	uint64_t r8;
	uint64_t rbp;
	uint64_t rdi;
	uint64_t rsi;
	uint64_t rdx;
	uint64_t rcx;
	uint64_t rbx;
	uint64_t rax;
	uint64_t error_code;
	uint64_t rip;
	uint64_t cs;
	uint64_t eflags;
	uint64_t rsp;
	uint64_t ss;
}__attribute__((packed));

//These are for handlers that dont push error code onto the stack. Like timer , keyboard etc.

struct isr_reg_handlers{
	uint64_t flags;
	uint64_t r15;
	uint64_t r14;
	uint64_t r13;
	uint64_t r12;
	uint64_t r11;
	uint64_t r10;
	uint64_t r9;
	uint64_t r8;
	uint64_t rbp;
	uint64_t rdi;
	uint64_t rsi;
	uint64_t rdx;
	uint64_t rcx;
	uint64_t rbx;
	uint64_t rax;
	uint64_t rip;
	uint64_t cs;
	uint64_t eflags;
	uint64_t rsp;
	uint64_t ss;
}__attribute__((packed));

extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();


extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();
extern void isr32();
extern void isr33();
extern void isr128();

extern void isr_others();

void clear_pic_interrupt();
void handle_isr_others();
void handle_isr0();
void handle_isr1();

void generic_handler();

 void handle_isr0();
 void handle_isr1();
 void handle_isr2();
 void handle_isr3();
 void handle_isr4();
 void handle_isr5();
 void handle_isr6();
 void handle_isr7();
 void handle_isr8();
 void handle_isr9();
 void handle_isr10();
 void handle_isr11();
 void handle_isr12();
 void handle_isr13();
 void handle_isr14();
 void handle_isr15();
 void handle_isr16();
 void handle_isr17();
 void handle_isr18();
 void handle_isr19();
 void handle_isr20();
 void handle_isr21();
 void handle_isr22();
 void handle_isr23();
 void handle_isr24();
 void handle_isr25();
 void handle_isr26();
 void handle_isr27();
 void handle_isr28();
 void handle_isr29();
 void handle_isr30();
 void handle_isr31();
 void handle_isr32();
 void handle_isr33();
 void handle_isr128();

#endif
