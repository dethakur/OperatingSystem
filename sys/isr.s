.macro interrupt_num interrupt_index

.global isr\interrupt_index
isr\interrupt_index:
	cli
	push %rax
	push %rbx
	push %rcx
	push %rdx
	push %rsi
	push %rdi
	push %rbp
	push %r8 
	push %r9 
	push %r10
	push %r11
	push %r12
	push %r13
	push %r14
	push %r15
	pushf
	call handle_isr\interrupt_index
	popf
	pop %r15
	pop %r14
	pop %r13
	pop %r12
	pop %r11
	pop %r10
	pop %r9 
	pop %r8 
	pop %rbp
	pop %rdi
	pop %rsi
	pop %rdx
	pop %rcx
	pop %rbx
	pop %rax
	sti
	iretq
.endm

interrupt_num 0
interrupt_num 1
interrupt_num 2
interrupt_num 3
interrupt_num 4
interrupt_num 5
interrupt_num 6
interrupt_num 7
interrupt_num 8
interrupt_num 9
interrupt_num 10
interrupt_num 11
interrupt_num 12
interrupt_num 13
interrupt_num 15
interrupt_num 16
interrupt_num 17
interrupt_num 18
interrupt_num 19
interrupt_num 20
interrupt_num 21
interrupt_num 22
interrupt_num 23
interrupt_num 24
interrupt_num 25
interrupt_num 26
interrupt_num 27
interrupt_num 28
interrupt_num 29
interrupt_num 30
interrupt_num 31
interrupt_num 32
interrupt_num 33
interrupt_num 128


.global isr14
isr14:
	cli
	push %rax
	push %rbx
	push %rcx
	push %rdx
	push %rsi
	push %rdi
	push %rbp
	push %r8 
	push %r9 
	push %r10
	push %r11
	push %r12
	push %r13
	push %r14
	push %r15
	pushf
	call handle_isr14
	popf
	pop %r15
	pop %r14
	pop %r13
	pop %r12
	pop %r11
	pop %r10
	pop %r9 
	pop %r8 
	pop %rbp
	pop %rdi
	pop %rsi
	pop %rdx
	pop %rcx
	pop %rbx
	pop %rax
	add $8, %rsp
	sti
	iretq
