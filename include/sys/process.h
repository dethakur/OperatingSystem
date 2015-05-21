#include<sys/defs.h>
#include<sys/isr.h>

#ifndef _PROCESS_H_
#define _PROCESS_H_

#define MAX_PROCESS 100
#define DEFAULT_MEMORY 400
#define U_STACK 0xFFFFEFFF 
#define U_ENVP 0xFFFFF000 
#define U_HEAP 0x0000A000 
#define MAX_OPEN_FILES 50

struct vm_area_struct{
	uint64_t start_kernel;
	uint64_t start_user;
	int64_t file_size;
	int file_backed;
	struct vm_area_struct *next;
	int permission;
	struct mm_struct *mma;
	int mem_allocated;
	uint64_t phys_addr;
	int is_stack_mem;
	uint64_t self_memory;
	int64_t mem_size;
};

typedef struct vm_area_struct vma; 
struct fdtable_str{
	void *fileptr;
	int64_t file_offset;
	int flags;
	char filetype;
	int64_t filesize; //used only for pipes
};  
typedef struct fdtable_str fd_obj;
struct mm_struct{
	uint64_t stack;
	uint64_t mmap_base; //Stores top to down. Allocate propoerly
	uint64_t heap;
	uint64_t start_code,end_code;
	uint64_t start_data,end_data;
	uint64_t vma_count;
	uint64_t cache;
	uint64_t self_memory;
	vma *vma_map;
};
typedef struct mm_struct mm;

struct registers{
	uint64_t r15;
	uint64_t r14;
	uint64_t r13;
	uint64_t r12;
	uint64_t r11;
	uint64_t r10;
	uint64_t r9;
	uint64_t r8;
	uint64_t rbp;
	uint64_t rdi;
	uint64_t rsi;
	uint64_t rdx;
	uint64_t rcx;
	uint64_t rbx;
	uint64_t rax;
	uint64_t rip;
	uint64_t cs;
	uint64_t eflags;
	uint64_t rsp;
	uint64_t ss;
}__attribute__((packed));	
int no_of_process;
typedef struct registers registers;
typedef enum {TASK_RUNNING,TASK_INTERRUPTIBLE,TASK_UNINTERRUPTIBLE,TASK_ZOMBIE,TASK_STOPPED,TASK_SWAPPING,TASK_WAITING,TASK_TERMINATED} task_state;

typedef enum {file_IO,kb_IO,sleepMode,child} w_param;
struct wait_str{
	w_param type;
	uint64_t value;
};
typedef struct wait_str wait_type;
struct pcb_block{
	registers reg;
	uint64_t cr3;
	uint64_t pid;
	uint64_t ppid;
	uint64_t timer_val;
	uint64_t start_exec;
	uint64_t self_memory;
	uint64_t env_set;
	task_state state;
	uint64_t env_mem;
	int maxfd;
	fd_obj* fdtable; 
	struct pcb_block* pp;
	struct pcb_block* child_process;
	void* kernel_stack;
	char *user_mem;
	char p_name[100];
	wait_type w_type;
	uint64_t p_cache;
	int is_background;
	mm* memory;
};

typedef struct pcb_block process;
process* create_process(process*,char*);
process* create_process_new(process*,char*,int create_fd);
process* init_process;
uint64_t create_process_page_table();
void clear_registers();
void start_process(volatile process*);
void create_init_process();
void test_process();
void delete_process(process*);
void mmap_process_page_table(uint64_t,uint64_t);
void switch_process(process*);
void write_tss(process*);
void set_kernel_stack(uint64_t address);
extern void tss_flush();
vma* create_vma_object(uint64_t,uint64_t,int64_t,int64_t,int,int,int);
mm* create_mm_object(process*);
void allocate_VMA_memory(uint64_t,uint64_t,int64_t,int64_t,int,process*);
void run_process(volatile process*);
uint64_t create_user_process_table();
void create_STACK_memory(mm*,uint64_t);
void save_process_contents(struct isr_reg_handlers,process*);
void switch_to(volatile process*);
void prempt(int swap);
extern void prempt_test();
int get_process_id();
void kill_process(volatile process*);
void out_of_memory();
#endif
