#include<sys/sbunix.h>
#include<sys/paging.h>
#include<sys/kmalloc.h>
#include<sys/process.h>

void* k_malloc(uint64_t size){
	uint64_t page_size = PAGE_SIZE;
	uint64_t pages = 0;
	if(size<=page_size){
		pages = 1;
	//	printf("~~~~Page asked is 1~~~~");
	}else{
		pages = size/page_size;
		pages = pages +1;
	//	printf("~~~~Pages asked for is %d ~~~~~",pages);	
	}

	int i =0;
	int page_needed = pages;
	uint64_t base_address = -1;
	uint64_t page_index_used[page_needed];

	for(i=0;i<NO_OF_FREE_PAGES;i++){
		if(pageArr[i].reference_count == 0){
			page_needed -=1; 
			base_address = (base_address == -1 ? pageArr[i].base_address : base_address);
			page_index_used[page_needed] = pageArr[i].base_address ;
		}else{
			page_needed = pages;
			base_address = -1;
		}

		if(page_needed == 0){
			int i=0;
			for(i=0;i<pages;i++){
				mark_physical_page_used(page_index_used[i]);
			}
			break;
		}

	}
	base_address = base_address + VIRTUAL_BASE_ADDRESS;
	//printf("~~~~~~~ Base address returned by kmalloc %x ~~~~~~\n",base_address);
	return (void*)base_address;
}

void* k_malloc_stack(int size){
	uint64_t address = (uint64_t)k_malloc(size);
	address = address + size;
	return (void*)address;
}
