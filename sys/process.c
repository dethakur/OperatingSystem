#include<sys/defs.h>
#include<sys/sbunix.h>
#include<sys/paging.h>
#include<sys/kmalloc.h>
#include<sys/gdt.h>
#include<sys/tarfs.h>
#include<sys/schedule.h>
#include<sys/process.h>
#include<sys/isr.h>
#include<sys/util.h>
#include<sys/pmap.h>

extern void _load_TR();
static int process_id=1;
extern int no_of_process;
extern void* physfree_kernel;
extern void* physbase_kernel;
volatile process* current_running_process = NULL;

process *first = NULL;
process *second = NULL;
static int task_loaded = 0;

int get_process_id(){
	int y =  process_id;
	process_id +=1;
	return y;
}

void save_process_contents(volatile struct isr_reg_handlers reg, process* p){
	if (p == NULL){
		return;
	}
	p->reg.rax = reg.rax;
	p->reg.rbx = reg.rbx;
	p->reg.rcx = reg.rcx;
	p->reg.rdx = reg.rdx;
	p->reg.rsi = reg.rsi;
	p->reg.rdi = reg.rdi;
	p->reg.rbp = reg.rbp;
	p->reg.r8  = reg.r8 ;
	p->reg.r9  = reg.r9 ;
	p->reg.r10 = reg.r10;
	p->reg.r11 = reg.r11;
	p->reg.r12 = reg.r12;
	p->reg.r13 = reg.r13;
	p->reg.r14 = reg.r14;
	p->reg.r15 = reg.r15;
	p->reg.rip = reg.rip;
	p->reg.cs = reg.cs;
	p->reg.eflags = reg.eflags | 0x200;
	p->reg.rsp = reg.rsp;
	p->reg.ss = reg.ss;
}


void switch_to(volatile process* current_process){
	//current_running_process = current_process;
	//enable_paging(current_process->cr3);
	start_process(current_process);
	volatile registers reg = current_process->reg;	
	__asm __volatile__("movq %0, %%rsp;"
			"pop %%r15;"
			"pop %%r14;"
			"pop %%r13;"
			"pop %%r12;"
			"pop %%r11;"
			"pop %%r10;"
			"pop %%r9;"
			"pop %%r8;"
			"pop %%rbp;"
			"pop %%rdi;"
			"pop %%rsi;"
			"pop %%rdx;"
			"pop %%rcx;"
			"pop %%rbx;"
			"pop %%rax;"
			"iretq;"
			:
			:"r"(&reg)
			:
			);

}

process* create_process(process* pp, char* bin_file){
	return create_process_new(pp,bin_file,1);
}


process* create_process_new(process* pp,char* bin_file,int create_fd){
	void *addr = k_malloc(sizeof(process));
	process *p = addr; 
	p->pid = process_id;
	process_id += 1;
	p->ppid = pp->pid;
	p->timer_val =0;
	p->state = TASK_INTERRUPTIBLE;
	p->memory = NULL; 
	p->self_memory = (uint64_t)addr;
	p->pp= pp;
	clear_registers(p->reg);
	p->cr3 = create_user_process_table();
	tarfs_init(bin_file,p);
	create_STACK_memory(p->memory,U_STACK);
	p->kernel_stack = k_malloc_stack(4095);
	p->env_set = 0;
	if(create_fd == 1){
		p->fdtable = k_malloc(sizeof(fd_obj)*50);
		p->maxfd=2;  //0,1,2 are reserved for stdin,stdout, and stderr respectively.
		p->fdtable[0].fileptr=NULL;
		p->fdtable[0].file_offset=0;
		p->fdtable[0].flags=O_RDONLY;  //std input is read only
		p->fdtable[0].filetype='S'; //standard i/o
		p->fdtable[1].fileptr=NULL;
		p->fdtable[1].file_offset=0;
		p->fdtable[1].flags=O_WRONLY;  //std output is write only
		p->fdtable[1].filetype='S'; //standard i/o
	}
	//	uint64_t* a = (uint64_t*)p->kernel_stack;
	//	a[0] = 24;
	//////print("Working === %x",p->kernel_stack);
	//////print("Process created with id = %d ",p->pid);
	//	run_process(p);
	//	//print("strlen = %d",strlen(bin_file));
	//	//print("bin_file = %s",bin_file);
	int size = strlen(bin_file);
	size = size > 50 ? 49:size;
	memcpy(bin_file,p->p_name,size);
	//	//print("file name  = %s",p->p_name);
	add_to_readyQ(p);	
	return p;

}

uint64_t create_user_process_table(){
	uint64_t cr3_content = init_paging();
	uint64_t* ptr = (uint64_t*)cr3_content;
	uint64_t address = VIRTUAL_BASE_ADDRESS;
	uint64_t* oldcr3 = (uint64_t*)init_process->cr3;
	int pml4e_index = getpml4e_index(address);
	ptr[pml4e_index] = oldcr3[pml4e_index];
	return cr3_content;
}

uint64_t create_process_page_table(){
	uint64_t cr3_content = init_paging();
	mapKernelMemory(physbase_kernel,physfree_kernel,cr3_content);
	return cr3_content;
}

void mmap_process_page_table(uint64_t cr3_content,uint64_t virtual_address){
	uint64_t physical_address = virtual_address - VIRTUAL_BASE_ADDRESS;
	createPageTableEntry(virtual_address,physical_address,cr3_content);
}

void delete_process(process *p){
	//__asm volatile("cli");
	////////print_page_count();
	process* pp = p->pp;
	start_process(pp);
	//free_page((uint64_t)p->kernel_stack);
	//deletePageTable(p->cr3);
	////////print_page_count();
	__asm volatile("sti");
}

void switch_process(process *p){
	start_process(p);
	//_load_TR();
	//////print("~~Process started with pid = %d ~~",current_running_process->pid);
}

void start_process(volatile process *p){
	//////print("~~~~ Process started with id = %d and CR3 is %x ~~~~",p->pid,p->cr3);
	enable_paging(p->cr3);
	current_running_process = p;
	p->state = TASK_RUNNING;
	//////print("Process started suckaaaaa %d",p->pid);
}

void run_process(volatile process* p){
	start_process(p);
	tss.rsp0 = (uint64_t)p->kernel_stack;
	if(task_loaded ==0){
		task_loaded = 1;
		_load_TR();
	}
	__asm__ volatile(
			"push $0x23;"
			"push %1;"
			//"push %%rax;"	
			"push $0x200;"
			"push $0x1B;"
			"push %0;"
			"iretq;"
			:
			:"r"(p->start_exec),"r"(p->memory->stack)
			:"memory","rax","rbx"
			);
}

void test_process(){
	//	process *p = create_process(init_process);
	//	start_process(p);
	//	//////print("Inside test process \n");
	//	__asm("cli");	
	//	//////print("~~~~~~~~~new process started~~~~~~~~~~");
}

void write_tss(process *p){
	uint64_t kernel_stack = (uint64_t)p->kernel_stack;
	tss.rsp0 = kernel_stack;

}

void map_process_memory(uint64_t virtual_address,process *p){
	//This method should always be called in kernel mode. Ring 0
	uint64_t physical_address = virtual_address - VIRTUAL_BASE_ADDRESS;
	uint64_t cr3_content = p->cr3;	
	createPageTableEntry(virtual_address,physical_address,cr3_content);	
	return;
}

void create_init_process(){
	process *p = k_malloc(sizeof(process));
	p->pid = process_id;
	p->ppid = process_id;
	process_id += 1;
	p->timer_val =0;
	p->state = TASK_INTERRUPTIBLE;
	p->memory = NULL; 
	p->kernel_stack = k_malloc_stack(4095);
	clear_registers(p->reg);
	p->cr3 = create_process_page_table();
	p->fdtable = k_malloc(sizeof(fd_obj)*50);
	p->maxfd=2;  //0,1,2 are reserved for stdin,stdout, and stderr respectively.
	p->fdtable[0].fileptr=NULL;
	p->fdtable[0].file_offset=0;
	p->fdtable[0].flags=O_RDONLY;  //std input is read only
	p->fdtable[0].filetype='S'; //standard i/o
	p->fdtable[1].fileptr=NULL;
	p->fdtable[1].file_offset=0;
	p->fdtable[1].flags=O_WRONLY;  //std output is write only
	p->fdtable[1].filetype='S'; //standard i/o
	tarfs_init("bin/init_process",p);
	//tarfs_init("bin/sbush",p);
	create_STACK_memory(p->memory,U_STACK);
	memcpy("bin/init_process",p->p_name,50);
	//////print("Working === %x",p->kernel_stack);
	//////print("Process created with id = %d ",p->pid);
	//	run_process(p);
	init_process = p;
}

void clear_registers(registers reg){
	reg.rax = 0;
	reg.rbx = 0;
	reg.rcx = 0;
	reg.rdx = 0;
	reg.rdi = 0;
	reg.rbp = 0;
	reg.r8  = 0;
	reg.r9  = 0;
	reg.r10 = 0;
	reg.r11 = 0;
	reg.r12 = 0;
	reg.r13 = 0;
	reg.r14 = 0;
	reg.r15 = 0;
	reg.rsi = 0;
	reg.rip = 0;
}


vma* create_vma_object(uint64_t start_user,uint64_t start_kernel,int64_t msize, int64_t fsize,int perm,int file_backed,int is_stack){
	void* addr =  k_malloc(sizeof(vma)); 
	vma* obj = addr;
	obj->self_memory = (uint64_t)addr;
	obj->start_user = start_user;
	obj->start_kernel = start_kernel;
	obj->file_size = fsize;
	obj->mem_size = msize;
	obj->permission = perm;
	obj->next = NULL;
	obj->mem_allocated = 0;
	obj->file_backed = file_backed;
	obj->phys_addr = 0;
	obj->is_stack_mem = is_stack;
	//	////print("Created vma for address = %x and %x",start_user,start_kernel);
	return obj;
}

void create_STACK_memory(mm* mm_obj,uint64_t stack_memory){
	mm_obj->stack = stack_memory;
	vma* obj = mm_obj->vma_map;
	while(obj->next != NULL){
		obj = obj->next;
	}
	vma* new_obj = create_vma_object(align_4KB(stack_memory),0,PAGE_SIZE,PAGE_SIZE,2,0,1);
	new_obj->is_stack_mem = 1;
	obj->next = new_obj; 
}

mm* create_mm_object(process* p){
	void* addr = k_malloc(sizeof(mm));
	mm* obj = addr; 
	obj->self_memory = (uint64_t)addr;
	obj->vma_count = 0;
	obj->vma_map = NULL;
	obj->heap = U_HEAP;
	obj->cache = (uint64_t)k_malloc(PAGE_SIZE);
	return obj;
}

void allocate_VMA_memory(uint64_t start_kernel,uint64_t start_user,int64_t fsize,int64_t msize,int perm,process* p){
//	print("[allocate_VMA_memory] PID = %d start_k = %x , start_user = %x , fsize = %x and msize = %x\n",p->pid,start_kernel,start_user,fsize,msize);
	if(p->memory == NULL){
		p->memory = create_mm_object(p);
	}
	while(msize > 0){
		int file_backed = 1;
		if(start_kernel ==0)
			file_backed = 0;
		int max_m_size = PAGE_SIZE;
		max_m_size = max_m_size > msize ? msize : max_m_size;

		int max_f_size = PAGE_SIZE;
		max_f_size = max_f_size > fsize ? fsize : max_f_size;

		if(p->memory->vma_count == 0){
			p->memory->vma_map = create_vma_object(start_user,start_kernel,max_m_size,max_f_size,perm,file_backed,0);
		}else{
			vma* obj = p->memory->vma_map;
			while(obj->next != NULL){
				obj = obj->next;
			}
			obj->next = create_vma_object(start_user,start_kernel,max_m_size,max_f_size,perm,file_backed,0); 
		}
		p->memory->vma_count +=1 ;
		//		create_empty_page((uint64_t)start_user,p->cr3);
		//		memcpy((char*)start_kernel,(char*)start_user,max_size);
//		print("Allocated VMA = %x and kernel = %x and fsize = %x and memesize %x",start_user,start_kernel,fsize,msize);
		start_user = start_user + PAGE_SIZE;
		start_kernel = start_kernel + PAGE_SIZE;
		fsize = fsize - (uint64_t)PAGE_SIZE;
		fsize = fsize<0?0:fsize;
		start_kernel = fsize ==0?0:start_kernel;	
		msize = msize - (uint64_t)PAGE_SIZE;
	}

}
void prempt(int swap){
	volatile process* p = get_ready_process();	
	//	//print("Trying to execute process = %s with id = %d",p->p_name,p->pid);
	if(p==NULL)
		return;

	if(current_running_process->state == TASK_UNINTERRUPTIBLE){
		return;
	}

	if(current_running_process->state == TASK_ZOMBIE){
		add_to_zombie_queue(current_running_process);
	}

	if(current_running_process->state != TASK_WAITING && current_running_process->state != TASK_ZOMBIE){
		current_running_process->state = TASK_SWAPPING;
		add_to_readyQ(current_running_process);
	}
	//disp_queue();
	//	disp_queue();
	if(p == NULL){
		//print("Process returned null value");
		return;
		//	switch_to(init_process);
	}else{
		if(p->reg.rsi != 0){
			switch_to(p);
		}else{
			run_process(p);
		}
	}

}
void kill_process(volatile process* p){
	invalidate_r_queue(p->pid);
	invalidate_w_queue(p->pid);
	child_update(p->pid);
	p->state = TASK_ZOMBIE;
	prempt(0);
}

void out_of_memory(){
	for(int i=0;i<20;i++){
		volatile process *p = get_ready_process();
		invalidate_r_queue(p->pid);
		invalidate_w_queue(p->pid);
		child_update(p->pid);
		free_process_memory(p);
	}
	volatile process *p = get_ready_process();
	if(p->reg.rsi != 0){
		switch_to(p);
	}else{
		run_process(p);
	}
}

