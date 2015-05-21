#include<sys/process.h>
#include<sys/process.h>
#include<sys/defs.h>
#include<sys/sbunix.h>

uint64_t fork();
void execve(process*,char*,char**,char**);
process* create_child_process(process*);
void copy_vma(process*,process*);
void copy_memory(process*,process*);
void set_COW_bits(process*,process*);
void copy_registers(process*,process*);
void copy_fdtable(process* parent,process* child);
int is_background_process(char**);
extern process* current_running_process;
vma* create_dup_vma(vma*); 

