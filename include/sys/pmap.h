#include<sys/defs.h>
#include<sys/process.h>
#include<sys/sbunix.h>
#include<sys/paging.h>

vma* get_vma_by_address(uint64_t,mm*);
void mark_vma_allocated(vma*,uint64_t);
void copy_data_to_vma(vma*);
void allocate_heap(int64_t, process*);
void free_process_memory(volatile process*);
void set_env(process*,char**,char**);
void copy_env(process*,char**,char**);
void disp_mem(char*,uint64_t);
