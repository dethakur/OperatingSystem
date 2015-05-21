.global jump_user_mode1
jump_user_mode1:
	movq $0x23, %rdx
	movq %rdx, %ds
	movq %rdx, %es
	movq %rdx, %fs
	movq %rdx, %gs

	movq %rsp, %rax
	push $0x23
	push %rax
	pushf
	push $0x1B
	push test_process
	iret


.global jump_user_mode
jump_user_mode:
	mov 0x64(%rsp),%rax
	push %rax
	ret

.global prempt_test
prempt_test:
	pop %rax
	pop %rax
	pop %rax
	pop %rsi
	pop %rdi
	ret


