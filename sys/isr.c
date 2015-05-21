#include<sys/isr.h>
#include<sys/port.h>
#include<sys/sbunix.h>
#include<sys/timer.h>
#include<sys/keyboard.h>
#include<sys/schedule.h>
#include<sys/process.h>
#include<sys/paging.h>
#include<sys/syscall.h>
#include<sys/util.h>
#include<sys/fork.h>
#include<sys/pmap.h>
#include<sys/paging.h>
#include<sys/tarfs.h>
#include<sys/kmalloc.h>
#include<sys/pipe.h>
#include<errno.h>


extern char* env_arr[];
extern process* current_running_process; 
extern void insert_blank();
void clear_pic_interrupt(){
	outb(0x20,0x20);
}

/*void generic_handler(){
  print("fault occured");
  __asm__("hlt");
  }
  */

void handle_isr0(){
	print("handle_isr0 interrupt occured");
	__asm__("hlt");
}


void handle_isr1(){
	print("handle_isr1 interrupt occured");
	__asm__("hlt");
}

void handle_isr2(){
	print("handle_isr2 interrupt occured");
	__asm__("hlt");

}

void handle_isr3(){
	print("handle_isr3 interrupt occured");
	__asm__("hlt");

}

void handle_isr4(){
	print("handle_isr4 interrupt occured");
	__asm__("hlt");

}

void handle_isr5(){
	print("handle_isr5 interrupt occured");
	__asm__("hlt");

}

void handle_isr6(struct isr_reg_ec_handlers reg){
	//print("Page Fault Handler!!");
	print("Error code in isr6 =%x",reg.error_code);
	//print("PAGE FAULT!!!!!!");
	uint64_t address = 0;
	__asm__ volatile ("movq %%cr2, %0;"
			:"=r"(address)
			:
			:"rax"
			);
	if(address == 0)
		print("Trying to map zero address");
	print("Address not mapped %x for pid = %d",address,current_running_process->pid);
	__asm("hlt");

}

void handle_isr7(){
	print("handle_isr7 interrupt occured");
	__asm__("hlt");

}

void handle_isr8(){
	print("handle_isr8 interrupt occured");
	__asm__("hlt");

}

void handle_isr9(){
	print("handle_isr9 interrupt occured");
	__asm__("hlt");

}

void handle_isr10(){
	print("handle_isr10 interrupt occured");
	__asm__("hlt");

}

void handle_isr11(){
	print("handle_isr11 interrupt occured");
	__asm__("hlt");

}

void handle_isr12(){
	print("handle_isr12 interrupt occured");
	__asm__("hlt");

}

void handle_isr13(volatile struct isr_reg_ec_handlers reg){
	print("GPL Error code =%x\n",reg.error_code);

	process* p = current_running_process;
	uint64_t address = 0;
	__asm__ volatile("movq %%cr2, %0;"
			:"=r"(address)
			:
			:"rax"
			);
	printf("SEGV for PID =  %d accessing address =\n",p->pid);
	kill_process(current_running_process);
	if(address == 0){
		print("Trying to map zero address");
		__asm("hlt");
	}
	print("Address not mapped %x for pid = %d",address,current_running_process->pid);

}

void handle_isr14(volatile struct isr_reg_ec_handlers reg){
	////print("Page Fault Handler!!");
	uint64_t error_code = reg.error_code;
	////print("PAGE FAULT!!!!!!");
	 uint64_t address = 0;
	 process *p = current_running_process;
	__asm__ volatile("movq %%cr2, %0;"
			:"=r"(address)
			:
			:"rax"
			);
	if(address == 0){
		printf("Segmentation Fault = %d\n",p->pid);
		kill_process(p);
	}

	//int is_cow =0;
	//printf("~~~PF = %x~~~and ec= %x !!!",address,error_code);
	//	print("Error code =%x, addr = %x and pid = %d",reg.error_code,address,p->pid);

	if(p->env_set ==0){
		char* argv[1];
		argv[0] = p->p_name;
		argv[1] = NULL;
		set_env(p,argv,env_arr);
	}
	mm* mm_obj = p->memory;
	vma* vma_obj = get_vma_by_address(address,mm_obj); 

	uint64_t stack_address = (uint64_t)(mm_obj->stack & ALIGN_4KB_PAGE);
	uint64_t s_addr = address & ALIGN_4KB_PAGE;

	if(vma_obj == 0){
		printf("Illegal Access. Segmentation Fault = %d\n",p->pid);
		kill_process(p);
	}

	if(error_code ==7){
		//		is_cow = 1;
		//	print("Read only fault , is cow = %d",is_cow);
		//	print("Final mapp is %x = %x",address,vma_obj->phys_addr);
		int ref_c = get_reference_count(vma_obj->phys_addr);
		//	print("Reference count in stack = %d",ref_c);
		disable_cow(address,p->cr3);
		if(ref_c >1){
			//		print("Duplicating memory addresses");
			uint64_t aligned_addr = align_4KB(address);
			memcpy((char*)aligned_addr,(char*)mm_obj->cache,PAGE_SIZE);
			mem_check((char*)mm_obj->cache,(char*)aligned_addr,PAGE_SIZE);

			uint64_t phys_addr = create_empty_page(address,p->cr3);

			memcpy((char*)mm_obj->cache,(char*)aligned_addr,PAGE_SIZE);
			mem_check((char*)mm_obj->cache,(char*)aligned_addr,PAGE_SIZE);

			memset((char*)mm_obj->cache,PAGE_SIZE,0);
			mark_physical_page_unused(vma_obj->phys_addr);
			vma_obj->phys_addr = phys_addr;
			//		print("PHys addr = %x = %x",phys_addr,addr_check);
			//		printf("New Permission = %x",perm);
		}else{
			//			print("NOOOOTTTT Duplicating memory addresses");
		}
	}else{

		if(stack_address == s_addr){
			uint64_t phys_addr = create_empty_page((uint64_t)s_addr,p->cr3);
			mark_vma_allocated(vma_obj,phys_addr);	
			uint64_t new_stack = mm_obj->stack - PAGE_SIZE;
			create_STACK_memory(mm_obj,new_stack);
		}else{
			uint64_t phys_addr = create_empty_page((uint64_t)vma_obj->start_user,p->cr3);
			mark_vma_allocated(vma_obj,phys_addr);
			//memcpy((char*)vma_obj->start_kernel,(char*)vma_obj->start_user,vma_obj->file_size);
			copy_data_to_vma(vma_obj);

		}
	}
	//refresh_cr3();
	enable_paging(p->cr3);
}



void handle_isr15(){
	print("handle_isr15 interrupt occured");
	__asm__("hlt");

}

void handle_isr16(){
	print("handle_isr16 interrupt occured");
	__asm__("hlt");

}

void handle_isr17(){
	print("handle_isr17 interrupt occured");
	__asm__("hlt");

}

void handle_isr18(){
	print("handle_isr18 interrupt occured");
	__asm__("hlt");

}

void handle_isr19(){
	print("handle_isr18 interrupt occured");
	__asm__("hlt");

}

void handle_isr20(){
	print("handle_isr20 interrupt occured");
	__asm__("hlt");

}

void handle_isr21(){
	print("handle_isr21 interrupt occured");
	__asm__("hlt");

}

void handle_isr22(){
	print("handle_isr22 interrupt occured");
	__asm__("hlt");

}

void handle_isr23(){
	print("handle_isr23 interrupt occured");
	__asm__("hlt");

}

void handle_isr24(){
	print("handle_isr24 interrupt occured");
	__asm__("hlt");

}

void handle_isr25(){
	print("handle_isr25 interrupt occured");
	__asm__("hlt");

}

void handle_isr26(){
	print("handle_isr26 interrupt occured");
	__asm__("hlt");

}

void handle_isr27(){
	print("handle_isr27 interrupt occured");
	__asm__("hlt");

}

void handle_isr28(){
	print("handle_isr28 interrupt occured");
	__asm__("hlt");

}

void handle_isr29(){
	print("handle_isr29 interrupt occured");
	__asm__("hlt");

}

void handle_isr30(){
	print("handle_isr30 interrupt occured");
	__asm__("hlt");
}

void handle_isr31(){
	print("handle_isr31 interrupt occured");
	__asm__("hlt");

}
void handle_isr32(volatile struct isr_reg_handlers reg){
	//	print("ISR 32 occured");
	save_process_contents(reg,current_running_process);	
	clear_pic_interrupt();
	disp_timer();
	//	__asm__("hlt");
}


void handle_isr33(volatile struct isr_reg_handlers reg){
	save_process_contents(reg,current_running_process);	
	clear_pic_interrupt();
	display_char();
}

void handle_isr128(volatile struct isr_reg_handlers reg){
	 uint64_t isr_code = reg.rax;
	//printf(" ISr code = %d ",isr_code);
	volatile process* p = current_running_process;
	save_process_contents(reg,current_running_process);	
	if(isr_code == SYS_write){
		uint64_t fd = reg.rdi;
		uint64_t* buf = (uint64_t*)reg.rsi;
		uint64_t count = reg.rdx; 

		if ((p->fdtable[fd].flags!=O_WRONLY)&&(p->fdtable[fd].flags!=O_RDWR)) //no write permission 
			reg.rax=-1*(EACCES);  
		else{
			if  (p->fdtable[fd].filetype=='S') {  //write to std output
				if(p->is_background == 1){
					printf("\nBG process cant access stdout. Killed \n");
					kill_process(p);
				}
				int buf_count =0;
				for(buf_count =0; buf_count < count; buf_count++){
					char c = buf[buf_count];
					//printf("%c",c);
					putchar(c);                 
				}		
			}else if  (p->fdtable[fd].filetype=='P') {  //write to pipe
				//printf(" Writing to pipe..."); 
				uint64_t writtenbytes= pipe_write(fd,(void *)buf,(uint64_t)count);
				reg.rax=writtenbytes;
			}
		}
	}
	else if(isr_code == SYS_read){
		uint64_t fd = reg.rdi;
		uint64_t buffer = reg.rsi;
		uint64_t reqbytes = reg.rdx;

		process* p = current_running_process;
		//printf("\n Process %s called sys_read on fd %d ,fd0 type=%c",p->p_name,fd,p->fdtable[fd].filetype);

		if ((p->fdtable[fd].flags!=O_RDONLY)&&(p->fdtable[fd].flags!=O_RDWR)) //no read permission 
			reg.rax=-1*(EACCES);  
		else{ 
			if  (p->fdtable[fd].filetype=='S') {  //read from std input

				invalidate_r_queue(current_running_process->pid);
				if(is_STDIN_locked() == 0 && p->is_background == 0){
					//printf("STDIN locked = %d\n",p->pid);
					p->user_mem = (char*)buffer;
					p->w_type.type = file_IO;
					p->w_type.value = fd;
					add_to_waitQ(p);
					//disp_queue();
					p->state = TASK_WAITING;
				}else{
					printf("Read on STDIN NOT Allowed. Process Killed %d\n",p->pid);
					p->state = TASK_ZOMBIE;		
					kill_process(p);
				}
				prempt(0);
				__asm("sti");
				__asm("hlt");	
			}else if (current_running_process->fdtable[fd].filetype=='T') { //tarfs
				uint64_t readbytes= tarfs_read(fd,(void *)buffer,(uint64_t)reqbytes);
				reg.rax=readbytes;
			}else if (current_running_process->fdtable[fd].filetype=='P') { //pipe
				uint64_t readbytes= pipe_read(fd,(void *)buffer,(uint64_t)reqbytes);
				reg.rax=readbytes;

			} 
		}
	}
	else if(isr_code == SYS_brk){
		volatile uint64_t *ptr = &reg.rax;
		*ptr = p->memory->heap;
		 uint64_t new_addr = reg.rdi;
		 int64_t size = 0;
		if(new_addr != 0){
			uint64_t current_heap = p->memory->heap;
			size = new_addr - current_heap;
			if(size >0){
				allocate_heap(size,current_running_process);
				*ptr = p->memory->heap + size;
			}
		}
	/*	if(size <0){
		}
		else{
		}*/

	//	printf("1 last = %x\n and reg = %x\n",reg.rax,&reg.rax);
	//	printf("PID = %d and heap = %x\n",p->pid,reg.rax);

	}
	else if(isr_code == SYS_wait4){
		//		print("Waitpid called");
		uint64_t pid = reg.rdi;
		if(is_child_zombie(pid) == 1){
			delete_zombie(pid);
			print("Child is zombie. Nothing to wait\n");
			return;
		}
		uint64_t mem = reg.rsi;
		current_running_process->w_type.type = child;
		current_running_process->w_type.value = pid;
		current_running_process->user_mem = (char*)mem;
		add_to_waitQ(current_running_process);
		invalidate_r_queue(current_running_process->pid);
		//disp_queue();
		current_running_process->state = TASK_WAITING;
		prempt(0);
	}
	else if(isr_code == SYS_getppid){
		 volatile uint64_t* ptr = &reg.rax;
		*ptr = current_running_process->pp->pid;
		//	print("Value of  rax = %x",isr_code);
	}
	else if(isr_code == SYS_getpid){
		 volatile uint64_t* ptr = &reg.rax;
		*ptr = current_running_process->pid;
		//	print("Value of  rax = %x",isr_code);
	}
	else if(isr_code == SYS_exit){
		//print("1Exit calledi by process= %d",p->pid);
		child_update(current_running_process->pid);
		p->state = TASK_ZOMBIE;
		//kmm_objill_process(p);
		invalidate_r_queue(p->pid);
		if(p->ppid ==1)
			free_process_memory(p);
		prempt(0);
		__asm("sti");
		__asm("hlt");

	}
	else if(isr_code == SYS_nanosleep){
		//printf("Process slept = %d\n",p->pid);	
		 volatile uint64_t* input = &reg.rdi;
		uint64_t value = *((uint64_t*)input[0]);
		invalidate_r_queue(p->pid);
		if(value ==0){
			prempt(0);
		}else{
			current_running_process->w_type.type = sleepMode;
			current_running_process->w_type.value = value; 
			add_to_waitQ(current_running_process);
			p->state = TASK_WAITING;
			prempt(0);
		}
		__asm("sti");
		__asm("hlt");
	}
	else if(isr_code == SYS_fork){
		//		print("Called fork");
		uint64_t pid = fork();
		if(pid == -1)
			pid = -1*ENOMEM;

		 volatile uint64_t* ptr = &reg.rax;
		//		print("[Kernel] Child process = %d",pid);
		*ptr = pid;

	}else if(isr_code == SYS_execve){
		char* ptr = (char*)reg.rdi;	

		char** argv = (char**)((uint64_t*)reg.rsi);
		char** envp = (char**)((uint64_t*)reg.rdx);
//		printf("\n Process %s called execve ,fd0 type=%c",current_running_process->p_name,current_running_process->fdtable[0].filetype); 
		//print("Value of execve %s\n",ptr);
		int val = tarfs_open(ptr,O_RDONLY);
		if(val <0){
			//printf("Couldnot find tarfs");
			return;
		}

		//		printf("Execve called");
		invalidate_r_queue(p->pid);
		p->state = TASK_ZOMBIE;
		execve(current_running_process,ptr,argv,envp);
		prempt(0);
		//	print("It came here");
	}
	else if(isr_code == SYS_getcwd){
		char *buf = (char *)reg.rdi;
		uint64_t size= (uint64_t)reg.rsi;
		if (strlen(cwd)>size)
			reg.rax=0;
		else{
			memcpy(cwd,buf,size);    
			reg.rax=(uint64_t)buf;
		}   
	}
	//-------------------------------------------------  

	else if(isr_code == SYS_chdir){
		char *path = (char *)reg.rdi;
		reg.rax=tarfs_chdir(path);
	}

	//-------------------------------------------------  
	else if(isr_code==SYS_pipe){
		int *pipefd= (int *)reg.rdi;
		int ret=create_pipe(pipefd);
		reg.rax=ret;
	}
 //-------------------------------------------------

	else if(isr_code==SYS_dup){
		int oldfd= (int)reg.rdi;
		int ret=setup_dup(oldfd);
		reg.rax=ret;
	}
	//-------------------------------------------------

	else if(isr_code==SYS_dup2){
		int oldfd= (int)reg.rdi;
		int newfd= (int)reg.rsi;
		int ret=setup_dup2(oldfd,newfd);
		reg.rax=ret;
	}
 //-------------------------------------------------

	else if(isr_code==SYS_lseek){
		uint64_t fd= reg.rdi;
		uint64_t offset= reg.rsi;
    uint64_t whence= reg.rdx;
		off_t ret=tarfs_lseek((int)fd,(off_t)offset,(int)whence);
		reg.rax=(int)ret;
	}
	//-------------------------------------------------    
	else if(isr_code==SYS_open){
		//print("Called tarfs_open");
		char* pathname = (char *) reg.rdi;
		uint64_t flags = reg.rsi;
		int64_t fd= tarfs_open(pathname,flags);
		reg.rax=fd;
	}
	//-------------------------------------------------
	else if(isr_code==SYS_getdents){
		uint64_t fd = reg.rdi;
		uint64_t buf = reg.rsi;
		uint64_t bufsize = reg.rdx;
		uint64_t outsize= tarfs_getdents(fd,(void *)buf,(uint64_t)bufsize);
		reg.rax=outsize;
	}
	//-------------------------------------------------  
	else if(isr_code==SYS_close){
		uint64_t fd = reg.rdi;
		uint64_t retcode= tarfs_close(fd);
		reg.rax=retcode;

	}else if(isr_code == SYS_ps){
		disp_queue();
	}else if(isr_code == SYS_bg){
		//printf("Process with ID = %d pushed into background \n",p->pid);
		p->w_type.type = file_IO;
		p->w_type.value = 99999;
		add_to_waitQ(p);
		p->state = TASK_WAITING;

	}else if(isr_code == SYS_kill){
		//		printf("[Kernel] Killing process %d\n",reg.rdi);				
		volatile process* p = get_process_by_id(reg.rdi); 
		if(p != NULL){
			//	printf("[Kernel] Process found !!!\n");
			printf("\n Killed -9 %d\n",p->pid);
			kill_process(p);
			free_process_memory(p);
		}else{
			printf("[Kernel] No Process with this id %d\n",p->pid);
		}
	}else{
		printf("UNKNOW ISRCODE = %d",isr_code);
	}
//	printf(" 2 last = %x\n and reg = %x",reg.rax,&reg.rax);
}
