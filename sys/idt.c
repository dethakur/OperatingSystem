#include<sys/sbunix.h>
#include<stdarg.h>
#include<sys/idt.h>
#include<sys/isr.h>
#include<sys/port.h>

void set_idt_gate(uint64_t funcAddress,int index);
static idt_gate idtArr[256];
static idt_reg idtPtr;
extern void _x86_64_asm_lidt(uint64_t address);


void pic_remap(){
	outb(0x20,0x20);
	outb(0x20, 0x11);
	outb(0xA0, 0x11);
	outb(0x21, 0x20);
	outb(0xA1,0x28);
	outb(0x21, 0x04);
	outb(0xA1, 0x02);
	outb(0x21, 0x01);
	outb(0xA1, 0x01);
}

void set_idt_entry(){
	
	pic_remap();
 	set_idt_gate((uint64_t)&isr0,0);
 	set_idt_gate((uint64_t)&isr1,1);
 	set_idt_gate((uint64_t)&isr2,2);
 	set_idt_gate((uint64_t)&isr3,3);
 	set_idt_gate((uint64_t)&isr4,4);
 	set_idt_gate((uint64_t)&isr5,5);
 	set_idt_gate((uint64_t)&isr6,6);
 	set_idt_gate((uint64_t)&isr7,7);
 	set_idt_gate((uint64_t)&isr8,8);
 	set_idt_gate((uint64_t)&isr9,9);
 	set_idt_gate((uint64_t)&isr10,10);
 	set_idt_gate((uint64_t)&isr11,11);
 	set_idt_gate((uint64_t)&isr12,12);
 	set_idt_gate((uint64_t)&isr13,13);
 	set_idt_gate((uint64_t)&isr14,14);
 	set_idt_gate((uint64_t)&isr15,15);
 	set_idt_gate((uint64_t)&isr16,16);
 	set_idt_gate((uint64_t)&isr17,17);
 	set_idt_gate((uint64_t)&isr18,18);
 	set_idt_gate((uint64_t)&isr19,19);
 	set_idt_gate((uint64_t)&isr20,20);
 	set_idt_gate((uint64_t)&isr21,21);
 	set_idt_gate((uint64_t)&isr22,22);
 	set_idt_gate((uint64_t)&isr23,23);
 	set_idt_gate((uint64_t)&isr24,24);
 	set_idt_gate((uint64_t)&isr25,25);
 	set_idt_gate((uint64_t)&isr26,26);
 	set_idt_gate((uint64_t)&isr27,27);
 	set_idt_gate((uint64_t)&isr28,28);
 	set_idt_gate((uint64_t)&isr29,29);
 	set_idt_gate((uint64_t)&isr30,30);
 	set_idt_gate((uint64_t)&isr31,31);
 	set_idt_gate((uint64_t)&isr32,32);
 	set_idt_gate((uint64_t)&isr33,33);
 	
	set_idt_gate((uint64_t)&isr128,128);
	idtArr[128].dpl = 3;

	idtPtr.limit = (uint16_t)sizeof(idtArr); 
	idtPtr.base = (uint64_t)(idtArr);
	_x86_64_asm_lidt((uint64_t)&idtPtr);
}



void set_idt_gate(uint64_t funcAddress,int index){
	idtArr[index].offset1 = funcAddress & 0x0000ffff;
	idtArr[index].selector = 8;
	idtArr[index].ist = 0;
	idtArr[index].zero = 0;
	idtArr[index].zero2 = 0;
	unsigned type = 0xe; //1110 - value for interrupt gate
	idtArr[index].type = type;
	idtArr[index].p = 1;
	idtArr[index].dpl = 0;
	idtArr[index].reserved = 0;
	idtArr[index].offset2 = ((funcAddress >> 16)) & 0x0000ffff;
	idtArr[index].offset3 = ((funcAddress >> 32)) & 0xffffffff;
}
