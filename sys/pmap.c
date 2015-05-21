#include<sys/pmap.h>
#include<sys/util.h>
#include<sys/paging.h>
#include<sys/kmalloc.h>

extern process* current_running_process;

vma* get_vma_by_address(uint64_t address, mm* mm_obj){
	//	print("[get_vma_by_address] address = %x",address);
	address = align_4KB(address);
	vma* vma_obj = mm_obj->vma_map;
	int found = 0;
	while(vma_obj != NULL){
		uint64_t start_addr = align_4KB(vma_obj->start_user);
		int msize = vma_obj->mem_size;
		if((address >= start_addr)  && (address < (start_addr+msize))){	
			found = 1;
			break;
		}else{
		}
		vma_obj = vma_obj->next;
	}
	if(found == 0){
		//		print("Address doesnot have a vma for process = %s = %x",current_running_process->p_name,address);
		return 0;
	}
	return vma_obj;

}

void copy_data_to_vma(vma* vma_obj){
	char* start_user = (char*) vma_obj->start_user;	
	int64_t fsize = vma_obj->file_size;
	int64_t memsize = vma_obj->mem_size;
	//process *p = current_running_process;

	if(vma_obj->start_kernel != 0){
		//	print("[pmap.c] pname = %s vma = %x , fsize = %d and memsize = %d",p->p_name,start_user,fsize,memsize);
		char* start_kernel = (char*)vma_obj->start_kernel;
		memcpy(start_kernel,start_user,fsize);		

	}
	start_user = start_user + fsize;
	memset(start_user,0,memsize-fsize);
}

void mark_vma_allocated(vma* vma_obj,uint64_t phys_addr){
	vma_obj->mem_allocated =  1;
	vma_obj->phys_addr = phys_addr;
}

void allocate_heap(int64_t size, process* p){
	mm* mem_obj = p->memory;
	uint64_t base_addr = mem_obj->heap;
	base_addr = align_4KB(base_addr);

	while(base_addr < (mem_obj->heap + size)){ 
		if(get_vma_by_address(base_addr,mem_obj) == 0 && size > 0){
			//printf("Heap VMA Created \n");
			allocate_VMA_memory(0,base_addr,0,PAGE_SIZE,2,p);
		}
		base_addr = base_addr + PAGE_SIZE;
	}
	mem_obj->heap += size;
}


void free_process_memory(volatile process* p){

//	return;
	//	print_page_count();
	//	return;
	//	printf("\nProcess Id = %d , memory freed\n",p->pid);
	enable_paging(p->cr3);
	mm* mm_obj = p->memory;
	vma* vma_obj = mm_obj->vma_map;
	while(vma_obj != NULL){
		//		if(vma_obj->mem_allocated != 0)
		//			free_physical_page(vma_obj->start_user,p->cr3);

		uint64_t addr = vma_obj->self_memory;
		vma_obj = vma_obj->next;
		free_page(addr,p->cr3);
	}
	free_page(mm_obj->self_memory,p->cr3);
	free_page(p->self_memory,p->cr3);


	free_page((uint64_t)p->kernel_stack,p->cr3);
	if(p->p_cache != 0)
		free_page(p->p_cache,p->cr3);

	if(p->memory != NULL && p->memory->cache != 0)
		free_page(p->memory->cache,p->cr3);

	if(p->fdtable != NULL){
		//		free_page((uint64_t)p->fdtable,p->cr3);
	}
	//	print_page_count();
	//	printf("Now deleting the page table \n");
	//	uint64_t cr3_val = p->cr3;
	deletePageTable(p->cr3);
	enable_paging(current_running_process->cr3);
	//print_page_count();
	return;
}

void copy_env(process *p,char** argv,char** envp){
	if(p->p_cache == 0){
		p->p_cache = (uint64_t)k_malloc(4095);
	}
	memset((char*)p->p_cache,4095,0);
	int argc = 0;
	while(argv[argc] != NULL || argv[argc] != 0)
		argc++;

	uint64_t* envp_stack = (uint64_t*)p->p_cache;
	*envp_stack = argc;
	envp_stack++;
	*envp_stack = 0;
	envp_stack++;

	char* ptr = (char*)envp_stack;
	for(int i=0;i<argc;i++){
		strcpy(argv[i],ptr);
		ptr = ptr + strlen(argv[i]);
		*ptr = '\0';
		ptr++;
	}

	*ptr= 0;
	ptr++;
	for(int i=0;envp[i]!=0;i++){
		strcpy(envp[i],ptr);
		ptr = ptr + strlen(envp[i]);
		*ptr = '\0';
		ptr++;
	}

	*ptr = 0;

}
void disp_mem(char* address,uint64_t size){
	for(int i=0;i<size;i++){
		if(*address == 0){
			address++;
			continue;
		}
		if(*address == '\0'){
			printf("\n");
			address++;
			continue;
		}
		printf("%c",*address);
		address++;
	}
}

void set_env(process *p,char** argv,char** envp){

	if(p->env_set ==1)
		return;

	copy_env(p,argv,envp);
	enable_paging(p->cr3);
	uint64_t phys_addr = create_empty_page((uint64_t)U_ENVP,p->cr3);
	char* envp_stack = (char*)U_ENVP;

	//disp_mem((char*)p->p_cache,1000);
	memcpy((char*)p->p_cache,envp_stack,4095);
	memset((char*)p->p_cache,4095,0);

	//disp_mem(envp_stack,1000);
	p->env_mem = phys_addr;
	p->env_set = 1;
	enable_paging(current_running_process->cr3);

}
