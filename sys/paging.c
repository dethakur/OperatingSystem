#include<sys/paging.h>
#include<sys/sbunix.h>
#include<sys/process.h>
#include<sys/util.h>
#include<sys/paging.h>

extern char _binary_tarfs_start;
extern char _binary_tarfs_end;

extern process* current_running_process;
static int pages_created =0;

void create_free_page_list(uint64_t start, uint64_t end){
	//////print("---star memory address %x--",start);
	uint64_t base_address = start;
	base_address = base_address & ALIGN_4KB_PAGE; 
	//////print("--Size of each page = %d--",PAGE_SIZE);
	base_address = base_address + PAGE_SIZE;	
	//uint64_t last_base_address = 0;
	int i=0;
	for(i=0;i<NO_OF_FREE_PAGES;i++){
		pageArr[i].base_address = base_address + (i*PAGE_SIZE); 		
		pageArr[i].reference_count = 0;
		//	last_base_address = pageArr[i].base_address;
	}
	////print("-- value of last memory mapped = %x",last_base_address);
}

void start_paging(void* physbase, void* physfree){
	mark_kernel_pages_used(physbase,physfree);
	//	mark_kernel_pages_used(&(_binary_tarfs_start),&(_binary_tarfs_end));

	create_init_process();	
	cr3_content = init_process->cr3;
	map_all_memory_to_pageTables(cr3_content);
	checkLookUp(physbase,physfree,cr3_content);
//	_asm("sti");
}

void mark_kernel_pages_used(void* physbase, void* physfree){
	mark_physical_page_used(video_memory);
	uint64_t start = (uint64_t)physbase;
	uint64_t end = (uint64_t)physfree;

	end = end & ALIGN_4KB_PAGE;
	end = end + PAGE_SIZE;

	while(start <=end){
		mark_physical_page_used(start);
		start = start + PAGE_SIZE;
	}
}

void map_all_memory_to_pageTables(uint64_t cr3_content){
	for(int i=0;i<NO_OF_FREE_PAGES;i++){
		uint64_t address = pageArr[i].base_address;
		uint64_t phys_addr = address;
		uint64_t virtual_addr = address + VIRTUAL_BASE_ADDRESS;
		createPageTableEntry(virtual_addr,phys_addr,cr3_content);
	}

}

void mark_physical_page_used(uint64_t phys_address){
	int i =0;
	for(i = 0;i<NO_OF_FREE_PAGES;i++){
		if(pageArr[i].base_address == phys_address){
			pageArr[i].reference_count += 1;
		}
	}
	pages_created += 1;
}


uint64_t get_reference_count(uint64_t phys_address){
	//print("Phys addr ref count wanted = %x",phys_address);
	int i =0;
	uint64_t ref_count=0;
	for(i = 0;i<NO_OF_FREE_PAGES;i++){
		if(pageArr[i].base_address == phys_address){
			ref_count = pageArr[i].reference_count;
		}
	}
	return ref_count;
}

void mark_physical_page_unused(uint64_t phys_address){
	int i =0;
	for(i = 0;i<NO_OF_FREE_PAGES;i++){
		if(pageArr[i].base_address == phys_address){
			pageArr[i].reference_count -= 1;
		}
	}
	pages_created -= 1;

}
void checkLookUp(void* physbase, void* physfree,uint64_t cr3_content){
	uint64_t start = (uint64_t) physbase;
	uint64_t end = (uint64_t)physfree;
	uint64_t i = 0;
	start = start + VIRTUAL_BASE_ADDRESS;
	end = end + VIRTUAL_BASE_ADDRESS;
	int matched = 0;
	while(start <= end){
		if(check_mapping(start,cr3_content)==1)
			matched+=1;
		start = start + PAGE_SIZE;
		i+=1;
	}

	//	////print("----Matched = %d---",matched);
	//	////print("----Total pages = %d--",i);
	uint64_t video_memory_base = VIRTUAL_BASE_ADDRESS + video_memory;  
	uint64_t value_returned = check_mapping(video_memory_base,cr3_content);
	if((value_returned+VIRTUAL_BASE_ADDRESS) == video_memory_base){
		//		////print("------Video Mmeory Mapped-----");
	}
}

uint64_t align_4KB(uint64_t address){
	return address & ALIGN_4KB_PAGE;
}

uint64_t check_mapping(uint64_t address,uint64_t cr3_content){
	uint64_t *pml4e_block = (uint64_t*)(cr3_content );
	int index = getpml4e_index(address);

	uint64_t *pdtpe_block = (uint64_t*)get_linear_address(align_4KB(pml4e_block[index]));
	index = getpdpte_index(address);

	uint64_t *pde_block = (uint64_t*)get_linear_address(align_4KB(pdtpe_block[index]));
	index = getpde_index(address);

	uint64_t *pte_block = (uint64_t*)get_linear_address(align_4KB(pde_block[index]));
	index = getpte_index(address);

	uint64_t offset = address & 0xFFF;	
	uint64_t phys_addr = (pte_block[index] & ALIGN_4KB_PAGE) + offset;
	if(phys_addr == (address - VIRTUAL_BASE_ADDRESS)){
		phys_addr = 1;
		//////print("-- matched--");	
		//////print("Memory matched for %x = %x",address,phys_addr);
	}else{
		////print("Memory not matched for %x = %x",address,phys_addr);
		////print("--Not matched--");	
		phys_addr = 0;
	}
	return phys_addr;
}
void disable_cow(uint64_t address,uint64_t cr3_content){
	//print("Disabling cow for %x address",address);
	mark_page_cow(address,cr3_content,1);
	refresh_cr3();
}

void enable_cow(uint64_t address,uint64_t cr3_content){
	mark_page_cow(address,cr3_content,0);
	refresh_cr3();
}

void mark_page_cow(uint64_t address,uint64_t cr3_content,int disable){
	uint64_t perm = 0x000;
	if(disable == 0){
		//print("Old perm = %x",perm);
		perm = perm | COW_BITS | PTE_P | PTE_U;
		perm = perm & PTE_RO;
		//print("Disable = 0 New perm = %x",perm);
	}else{
		perm = 0x000;
		perm = perm | PTE_P | PTE_W | PTE_U;
		//print("Disable = 1New perm = %x",perm);
	}
	uint64_t *pml4e_block = (uint64_t*)(cr3_content );
	int index = getpml4e_index(address);
	pml4e_block[index] = align_4KB(pml4e_block[index]) | perm;

	uint64_t *pdtpe_block = (uint64_t*)get_linear_address(align_4KB(pml4e_block[index]));
	index = getpdpte_index(address);
	pdtpe_block[index] = align_4KB(pdtpe_block[index]) | perm;

	uint64_t *pde_block = (uint64_t*)get_linear_address(align_4KB(pdtpe_block[index]));
	index = getpde_index(address);
	pde_block[index] = align_4KB(pde_block[index]) | perm;

	uint64_t *pte_block = (uint64_t*)get_linear_address(align_4KB(pde_block[index]));
	index = getpte_index(address);
	pte_block[index] = align_4KB(pte_block[index]) | perm;

	//uint64_t offset = address & 0xFFF;	
	//uint64_t phys_addr = (pte_block[index]);

	//phys_addr = align_4KB(phys_addr);
	//phys_addr = phys_addr | perm;
	////print("User Space Address = %x",phys_addr);
	//pte_block[index] = phys_addr;
}

uint64_t refresh_pte_value(uint64_t address,uint64_t cr3_content){
	uint64_t *pml4e_block = (uint64_t*)(cr3_content );
	int index = getpml4e_index(address);

	uint64_t *pdtpe_block = (uint64_t*)get_linear_address(align_4KB(pml4e_block[index]));
	index = getpdpte_index(address);

	uint64_t *pde_block = (uint64_t*)get_linear_address(align_4KB(pdtpe_block[index]));
	index = getpde_index(address);

	uint64_t *pte_block = (uint64_t*)get_linear_address(align_4KB(pde_block[index]));
	index = getpte_index(address);

	//uint64_t offset = address & 0xFFF;	
	uint64_t addr = pte_block[index];
	addr = align_4KB(addr) | PTE_U | PTE_P | PTE_W;
	pte_block[index] = addr; 
	return addr;

}

uint64_t pte_block_value(uint64_t address,uint64_t cr3_content){
	uint64_t *pml4e_block = (uint64_t*)(cr3_content );
	int index = getpml4e_index(address);

	uint64_t *pdtpe_block = (uint64_t*)get_linear_address(align_4KB(pml4e_block[index]));
	index = getpdpte_index(address);

	uint64_t *pde_block = (uint64_t*)get_linear_address(align_4KB(pdtpe_block[index]));
	index = getpde_index(address);

	uint64_t *pte_block = (uint64_t*)get_linear_address(align_4KB(pde_block[index]));
	index = getpte_index(address);

	//uint64_t offset = address & 0xFFF;	
	return pte_block[index];
}

uint64_t check_page_mapping(uint64_t address,uint64_t physical_addr,uint64_t cr3_content){
	uint64_t *pml4e_block = (uint64_t*)(cr3_content );
	int index = getpml4e_index(address);

	uint64_t *pdtpe_block = (uint64_t*)get_linear_address(align_4KB(pml4e_block[index]));
	index = getpdpte_index(address);

	uint64_t *pde_block = (uint64_t*)get_linear_address(align_4KB(pdtpe_block[index]));
	index = getpde_index(address);

	uint64_t *pte_block = (uint64_t*)get_linear_address(align_4KB(pde_block[index]));
	index = getpte_index(address);

	//uint64_t offset = address & 0xFFF;	
	uint64_t phys_addr = (pte_block[index] & ALIGN_4KB_PAGE); //+ offset;
	if(phys_addr == physical_addr){
		//print("-- matched--");	
		//print("Memory matched for va = %x , acc =  %x , found = %x",address,phys_addr,physical_addr);
	}else{
		//print("Memory not matched for va = %x , acc =  %x , found = %x",address,phys_addr,physical_addr);
		//print("--Not matched--");	
		phys_addr = 0;
	}
	return phys_addr;
}


uint64_t init_paging(){
	uint64_t address = create_page_block(512); 
	uint64_t cr3_content = address;
	return cr3_content;
}

int64_t print_page_count(){
	//print("!!!!!1 Page used in the system = %d",pages_created);
	int i=0;
	int count =1;
	for(i=0;i<NO_OF_FREE_PAGES;i++){
		if(pageArr[i].reference_count == 1)
			count +=1;
	}
	return count;
	//print("Total pages actuallly used = %d\n",count);
}

void enable_paging(uint64_t cr3_content){
	uint64_t physical_addr = get_physical_address((uint64_t)cr3_content);
	//	__asm__("cli");	
	__asm__ volatile("mov %0, %%cr3;"

			:
			:"r"(physical_addr)
			:
			);
	//	__asm__("sti");
}

void mapKernelMemory(void* physbase,void* physfree,uint64_t cr3_content){

	uint64_t start = (uint64_t) physbase;
	uint64_t end = (uint64_t)physfree;
	uint64_t i = 0;
	while(start <= end){
		uint64_t linear_address = start + VIRTUAL_BASE_ADDRESS;
		uint64_t physical_address = start;
		createPageTableEntry(linear_address,physical_address,cr3_content);
		//////print("--start == %x--",start);
		start = start + PAGE_SIZE;
		i+=1;
	}

	uint64_t linear_video_memory = VIRTUAL_BASE_ADDRESS + video_memory;  

	createPageTableEntry(linear_video_memory,video_memory,cr3_content);
	////print("--pages mapped in memory = %d--",i);
}

uint64_t get_physical_address(uint64_t linear_address){
	return linear_address - VIRTUAL_BASE_ADDRESS; 
}

uint64_t get_linear_address(uint64_t physical_address){
	return physical_address + VIRTUAL_BASE_ADDRESS;
}

void createPageTableEntry(uint64_t address,uint64_t physical_address,uint64_t cr3_content){
	int pml4e_index = getpml4e_index(address);
	uint64_t *pml4e_block = (uint64_t*)(cr3_content );

	if(pml4e_block[pml4e_index] == 0){
		//////print("##pml4e--index= %d##",pml4e_index);
		uint64_t pdpte_block = create_page_block(512);
		pml4e_block[pml4e_index] = get_physical_address(pdpte_block & ALIGN_4KB_PAGE) | PTE_P | PTE_W | PTE_U;
	}

	uint64_t *pdtpe_block = (uint64_t*)get_linear_address(pml4e_block[pml4e_index] & ALIGN_4KB_PAGE) ;
	int pdtpe_index = getpdpte_index(address);
	if(pdtpe_block[pdtpe_index] == 0){
		//	////print("##-pdtpe--index= %d--##",pdtpe_index);
		uint64_t pde_block = create_page_block(512);
		pdtpe_block[pdtpe_index] = get_physical_address(pde_block & ALIGN_4KB_PAGE) | PTE_P | PTE_W | PTE_U;
	}

	uint64_t *pde_block = (uint64_t*)get_linear_address(pdtpe_block[pdtpe_index] & ALIGN_4KB_PAGE);
	int pde_index = getpde_index(address);
	if(pde_block[pde_index] == 0){
		//	////print("-@@pde--index= %d@@--",pde_index);
		uint64_t pte_block = create_page_block(512);
		pde_block[pde_index] = get_physical_address(pte_block & ALIGN_4KB_PAGE) | PTE_P | PTE_W | PTE_U;
	}

	uint64_t *pte_block = (uint64_t*)get_linear_address(pde_block[pde_index] & ALIGN_4KB_PAGE);
	int pte_index = getpte_index(address);
//	if(pte_block[pte_index] == 0){
		//	////print("-!!pte--index= %d!!--",pte_index);
	uint64_t pageBlock = physical_address ;
		//pageBlock = pageBlock & ALIGN_4KB_PAGE;
	pte_block[pte_index] = pageBlock | PTE_P | PTE_W | PTE_U;
//	}
}


uint64_t create_page_block(uint64_t size){
	uint64_t memory_block = get_free_page();
	return memory_block;
}

int getpml4e_index(uint64_t address){
	uint64_t var = address;
	var = var >> 39;
	var = var & BIT_BINARY_9;
	return var;
}

int getpdpte_index(uint64_t address){
	uint64_t var = address;
	var = var >> 30;
	var = var & BIT_BINARY_9;
	return var;
}

int getpte_index(uint64_t address){
	uint64_t var = address;
	var = var >> 12;
	var = var & BIT_BINARY_9;
	return var;
}

int getpde_index(uint64_t address){
	uint64_t var = address;
	var = var >> 21;
	var = var & BIT_BINARY_9;
	return var;
}

void deletePageTable(uint64_t cr3_content){
	int l1=0;
	int pages_deleted =0;
	uint64_t *pml4e = (uint64_t*)(align_4KB(cr3_content)); 
	for(l1=0;l1<1;l1++){
		if(pml4e[l1] == 0)
			continue;
		uint64_t *pdtpe = (uint64_t*)get_linear_address(align_4KB(pml4e[l1]));
		int l2=0;
		for(l2=0;l2<512;l2++){
			if(pdtpe[l2]==0)
				continue;
			uint64_t *pde = (uint64_t*)get_linear_address(align_4KB(pdtpe[l2]));
			int l3=0;
			for(l3=0;l3<512;l3++){
				if(pde[l3]==0)
					continue;
				uint64_t *pte = (uint64_t*)get_linear_address(align_4KB(pde[l3]));
				int l4 =0;

				for(l4 = 0; l4<512;l4++){
					   if(pte[l4] == 0)
							 continue;
					   free_physical_page(pte[l4],cr3_content);
					   pages_deleted +=1;
					   pte[l4] = 0;
				}
				pages_deleted +=1;
				free_physical_page(pde[l3],cr3_content);
				pde[l3]=0;

			}
			free_physical_page(pdtpe[l2],cr3_content);
			pdtpe[l2]=0;
			pages_deleted +=1;
		}
		free_physical_page(pml4e[l1],cr3_content);
		pml4e[l1]=0;
		pages_deleted +=1;
	}
	//free_page((uint64_t)pml4e);
//	pages_deleted +=1;
	//print("~~~~~ Pages deleted In page table= %d~~~~\n",pages_deleted);
}


void free_physical_page(uint64_t physical_address,uint64_t cr3_content){
	uint64_t v_addr = physical_address + VIRTUAL_BASE_ADDRESS;
	free_page(v_addr,cr3_content);
}

void free_page(uint64_t virtual_address,uint64_t cr3_content){
	uint64_t physical_address = refresh_pte_value(virtual_address,cr3_content);
	physical_address = align_4KB(physical_address);
	int i=0;
	for(i=0;i<NO_OF_FREE_PAGES;i++){
		if(pageArr[i].base_address == physical_address){
			if(pageArr[i].reference_count >0){
				mark_physical_page_unused(physical_address);
				//pageArr[i].freed=1;
			}else{
			//	print("%x = !!! EXISTSS~~~",physical_address);
			}
			break;
		}
	}
}
uint64_t get_free_page(){
	uint64_t free_memory = 0;
	int i=0;
	for(i=0;i<NO_OF_FREE_PAGES;i++){
		if(pageArr[i].reference_count == 0){
			free_memory = pageArr[i].base_address; 
			mark_physical_page_used(free_memory);
			break;
		}
	}
	if(free_memory == 0){
		print("Out of memory!!!\n");
		out_of_memory();
		__asm__("hlt");
	}
	uint64_t virtual_address = free_memory + VIRTUAL_BASE_ADDRESS;
	memset((char*)virtual_address,PAGE_SIZE,0);	
	return virtual_address;
}

uint64_t create_empty_page(uint64_t address,uint64_t cr3_content){
	address = align_4KB(address);
	uint64_t physical_addr = get_free_page() - VIRTUAL_BASE_ADDRESS;
//	print("[create_empty_page] mapping = %x to %x\n",address,physical_addr);
	createPageTableEntry(address,physical_addr,cr3_content);
	//		//	check_page_mapping(address,physical_addr,cr3_content);
	return physical_addr;			
}			
