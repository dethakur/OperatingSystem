#include<sys/kmalloc.h>
#include<sys/process.h>
#include<sys/paging.h>
#include<sys/fork.h>
#include<sys/schedule.h>
#include<sys/util.h>
#include<sys/pmap.h>

uint64_t fork(){
	//print("Forking Bitch");
	if(print_page_count() > 20000){
//		printf("Too Many Processes !\n");
		return -1;
	}
	process* child = create_child_process(current_running_process);
	add_to_readyQ(child);
	return child->pid;
}

process* create_child_process(process* parent){
	void *addr = k_malloc(sizeof(process));
	process* child = addr; 
	child->self_memory = (uint64_t)addr;
	child->pid = get_process_id();
	child->ppid = parent->pid;
	child->timer_val = 0;
	child->state = TASK_INTERRUPTIBLE;
	child->memory = NULL;
	child->pp = parent;
	child->p_cache = 0;
	child->kernel_stack = k_malloc_stack(4095);
	clear_registers(child->reg);
	child->cr3 = create_user_process_table();
	child->memory = create_mm_object(child);
	copy_vma(parent,child);
	copy_fdtable(parent,child);
	//child->fdtable = parent->fdtable; 
	child->maxfd = parent->maxfd;
	copy_memory(parent,child);
	set_COW_bits(child,parent);
	child->start_exec = parent->start_exec;
	copy_registers(child,parent);
	strcpy(parent->p_name,child->p_name);
	child->env_set =0;
	refresh_cr3();
	return child;

}
void set_COW_bits(process* child, process* parent){
	//	uint64_t* child_cr3 = (uint64_t*)child->cr3;
	//	uint64_t* parent_cr3 = (uint64_t*)parent->cr3;
	mm* mm_obj = parent->memory;
	vma* vma_obj = mm_obj->vma_map;
	while(vma_obj != NULL){
		//		uint64_t user_mem = vma_obj->start_user;
		//		int pml4e_index = getpml4e_index(user_mem);
		/*		if(child_cr3[pml4e_index] == 0){
		////print("allocated");
		child_cr3[pml4e_index] = parent_cr3[pml4e_index];
		}*/

		if(vma_obj->mem_allocated == 1 ){
			uint64_t phys_addr = vma_obj->phys_addr;
			uint64_t address = vma_obj->start_user;
			//	uint64_t o_perm = get_page_perm(address,parent->cr3);
			createPageTableEntry(address,phys_addr,child->cr3);
			//if(vma_obj->is_stack_mem == 1){
			//	memcpy(mm_obj->cache,address,PAGE_SIZE);		
			//}else{
			//print("VMA is allocatedi %x and phys_addr %x",address,phys_addr);
			mark_physical_page_used(phys_addr);

			enable_cow(address,child->cr3);	
			enable_cow(address,parent->cr3);	
			//	uint64_t perm = get_page_perm(address,parent->cr3);
			//print("Permission changed =from %x to  %x",o_perm,perm);

			//uint64_t child_val = get_page_phys_addr(address,child->cr3);
			//uint64_t parent_val = get_page_phys_addr(address,parent->cr3);
			//print("Physical _ address %x to  %x",child_val,parent_val);
			//	}
			//	}
	}
	vma_obj = vma_obj->next;
}
}

void copy_vma(process* parent,process* child){
	vma* p_vma = parent->memory->vma_map;
	while(p_vma != NULL){
		if(child->memory->vma_count == 0){
			child->memory->vma_map = create_dup_vma(p_vma);
		}else{
			vma* obj = child->memory->vma_map;
			while(obj->next != NULL)
				obj = obj->next;
			obj->next = create_dup_vma(p_vma); 
		}
		child->memory->vma_count += 1;
		p_vma = p_vma->next;
	}


}
vma* create_dup_vma(vma* vma_obj){
	void *addr = k_malloc(sizeof(vma));
	vma* obj = addr; 
	obj->start_user = vma_obj->start_user;
	obj->start_kernel = vma_obj->start_kernel;
	obj->file_size = vma_obj->file_size;
	obj->mem_size = vma_obj->mem_size;
	obj->self_memory = (uint64_t)addr;
	obj->permission = vma_obj->permission;
	obj->next = NULL;
	obj->mem_allocated = vma_obj->mem_allocated;
	obj->file_backed = vma_obj->file_backed;
	obj->phys_addr = vma_obj->phys_addr;
	obj->is_stack_mem = vma_obj->is_stack_mem;
	return obj;
}
void copy_memory(process* parent, process* child){
	mm* p_obj = parent->memory;
	mm* c_obj = child->memory;
	c_obj->stack = p_obj->stack;
	c_obj->heap = p_obj->heap;
	c_obj->vma_count = p_obj->vma_count;

	//	enable_paging(child->cr3);
	c_obj->cache = (uint64_t)k_malloc(PAGE_SIZE);
	//	enable_paging(parent->cr3);
}
void copy_registers(process* child,process* parent){
	child->reg.rax = parent->reg.rax;
	child->reg.rbx = parent->reg.rbx;
	child->reg.rcx = parent->reg.rcx;
	child->reg.rdx = parent->reg.rdx;
	child->reg.rsi = parent->reg.rsi;
	child->reg.rdi = parent->reg.rdi;
	child->reg.rbp = parent->reg.rbp;
	child->reg.r8  = parent->reg.r8; 
	child->reg.r9  = parent->reg.r9; 
	child->reg.r10 = parent->reg.r10;
	child->reg.r11 = parent->reg.r11;
	child->reg.r12 = parent->reg.r12;
	child->reg.r13 = parent->reg.r13;
	child->reg.r14 = parent->reg.r14;
	child->reg.r15 = parent->reg.r15;
	child->reg.rip = parent->reg.rip;
	child->reg.cs  = parent->reg.cs; 
	child->reg.eflags = parent->reg.eflags;
	child->reg.rsp = parent->reg.rsp;
	child->reg.ss  = parent->reg.ss; 
}

void copy_fdtable(process* parent,process* child){
	int i;
	fd_obj *cfd=(fd_obj *)k_malloc(sizeof(fd_obj)*50);
	child->fdtable=cfd;
	fd_obj *pfd=parent->fdtable;
	for(i=0;i<50;i++,cfd++,pfd++){
		*cfd=*pfd;
	}
}

int is_background_process(char* argv[]){
	int is_background = 0;
	int i =0;
	while(argv[i]!= NULL|| argv[i] != 0){
		if(strcmp(argv[i],"&")==0){
			is_background = 1;
			argv[i] = 0;
			break;
		}
		i++;
	}
	return is_background;
}
void execve(process* p, char* bin_file,char* argv[], char* envp[]){
  //printf("\n [in execve fn]Process %s called execve ,fd0 type=%c",p->p_name,p->fdtable[0].filetype);
	process* new_p = create_process_new(p->pp,bin_file,0);
	int is_background = is_background_process(argv);
	//new_p->fdtable = p->fdtable;
	new_p->maxfd = p->maxfd;
	invalidate_r_queue(p->pid);
	new_p->pid = p->pid;

	//set the environment variable of the new process
	set_env(new_p,argv,envp);
	copy_fdtable(p,new_p);
	free_process_memory(p);
	
	if(is_background == 1){
		new_p->is_background = 1;
		child_update(new_p->pid);
	}

	add_to_readyQ(new_p);

}



