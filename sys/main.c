#include <sys/sbunix.h>
#include <sys/gdt.h>
#include <sys/tarfs.h>
#include <sys/idt.h>
#include <sys/timer.h>
#include<sys/paging.h>
#include<sys/kmalloc.h>
#include<sys/tarfs.h>
#include<sys/schedule.h>
#include<sys/pmap.h>
#include<sys/util.h>

#define NAME_MAX 255
void* physbase_kernel;
void* physfree_kernel;
process* process1;
process* process2;

volatile char* env_arr[] = {"PATH=bin/","USER=dthakur","ENV=SUCKS",NULL};

void start(uint32_t* modulep, void* physbase, void* physfree)
{
	physbase_kernel = physbase;
	physfree_kernel = physfree;
	struct smap_t {
		uint64_t base, length;
		uint32_t type;
	}__attribute__((packed)) *smap;
	
	//print("-2 -physfree = %p--\n",physfree);
	//print("-1 -physbase = %p--\n",physbase);

	uint64_t phys_mem_avail_start = 0;
	uint64_t phys_mem_avail_end = 0;
	
	while(modulep[0] != 0x9001) modulep += modulep[1]+2;
	for(smap = (struct smap_t*)(modulep+2); smap < (struct smap_t*)((char*)modulep+modulep[1]+2*4); ++smap) {
		if (smap->type == 1 /* memory */ && smap->length != 0) {
			phys_mem_avail_start = smap->base;
			phys_mem_avail_end = smap->base+ smap->length;


		}

	}

//	print("Available Physical Memory [%x-%x]\n-----",phys_mem_avail_start, phys_mem_avail_end);
	set_idt_entry();
	init_timer();

	create_free_page_list(phys_mem_avail_start,phys_mem_avail_end);
		
//	print("tarfs in [%x:%x]\n", &_binary_tarfs_start, &_binary_tarfs_end);
	start_paging(physbase,physfree);
	create_process(init_process,"bin/sbush");
	//create_process(init_process,"bin/stdin");
	//create_process(init_process,"bin/ps");
	//create_process(init_process,"bin/exit");
	//create_process(init_process,"bin/hello3");
	cwd = (char*)k_malloc(NAME_MAX);
  	strcpy("/bin/",cwd);
	//__asm("sti");
	run_process(init_process);
//	create_process(init_process,"bin/hello4");
//	create_process(init_process,"bin/hello5");
	//process1 = get_ready_process();
	//process2 = get_ready_process();
	//run_process(process1);
/*	process *p = get_ready_process();
	if(p != NULL){
		print("Starting pid = %d",p->pid);
		run_process(p);	
	}*/
//	disp_queue();
	//start_process(p);
//	char* p = (char*)k_malloc(1024);
//	p[0]='d';
//	p[1] = 'e';
//	p[2] = 'v';
//	p[3] = 'a';
//	print("@@@@strval = %s @@@@@",p);
//	no_of_process = 1;
//	temp_process = create_process(init_process);
	//switch_process(init_process);
//	switch_process(temp_process);
	//process* test_process = create_process(init_process);
	//start_process(test_process);
	//delete_process(test_process);
	//jump_user_mode(123);	
	// kernel starts here
//	run_process(init_process);
	while(1);
				
}

#define INITIAL_STACK_SIZE 4096
char stack[INITIAL_STACK_SIZE];
uint32_t* loader_stack;
extern char kernmem, physbase;
struct tss_t tss;
struct page pageArr[NO_OF_FREE_PAGES];
void dummyPrint();
void boot(void)
{
	// note: function changes rsp, local stack variables can't be practically used
	//	register char *s; 
	__asm__(
			"movq %%rsp, %0;"
			"movq %1, %%rsp;"
			:"=g"(loader_stack)
			:"r"(&stack[INITIAL_STACK_SIZE])
	       );
	reload_gdt();
	setup_tss();
	//print("--pageArr beginning-- = %x --",&pageArr[0]);
	//print("--pageArr end -- = %x --",&pageArr[PAGE_SIZE]);
	//print("--loaderStack beginning-- = %x --",&stack[0]);
	//print("-- loaderStack end-- = %x --",&stack[INITIAL_STACK_SIZE]);
	start(
			(uint32_t*)((char*)(uint64_t)loader_stack[3] + (uint64_t)&kernmem - (uint64_t)&physbase),
			&physbase,
			(void*)(uint64_t)loader_stack[4]
	     );

	//dummyPrint();
	
	while(1);
}
void dummyPrint(){
  print("Output of console print: ");
	char c = 'c';
	print(" Character  = %c ----",c );
	int x = 10;
	print("Integer = %d , hex = %x and pointer address = %p",x,x,&x);
	char s[1000]="*********This console supports scrolling after 1080 characters. Type on keyboard to see the string appearing on the console *****";
	print("%s",s);
}
