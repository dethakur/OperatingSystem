#ifndef KMALLOC_H
#define KMALLOC_H 

#include<sys/defs.h>

void* k_malloc(uint64_t);
void* k_malloc_stack(int);
#endif


