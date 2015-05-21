#include<sys/defs.h>
struct idtr_entry{
	uint16_t limit;
	uint64_t base;
}__attribute((packed));

typedef struct idtr_entry idt_reg;

struct idt_gate_descriptor{
	uint16_t offset1;
	uint16_t selector;
	unsigned ist:3;
	unsigned zero:5;
	unsigned type:4;
	unsigned zero2:1;
	unsigned dpl:2;
	unsigned p:1;
	uint16_t offset2;
	uint32_t offset3;
	uint32_t reserved;

}__attribute((packed));

typedef struct idt_gate_descriptor idt_gate;
void set_idt_entry();
