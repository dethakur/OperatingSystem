#include<sys/defs.h>

#ifndef PAGING_H
#define PAGING_H

#define NO_OF_FREE_PAGES 30000  
#define PTE_P 0x001
#define PTE_W 0x002
#define PTE_U 0x004
#define ALIGN_4KB_PAGE  0xFFFFFFFFFFFFF000
#define BIT_BINARY_9 0b111111111
#define VIRTUAL_BASE_ADDRESS 0xffffffff80000000
#define video_memory 0xB8000
#define PAGE_SIZE 0x1000
#define COW_BITS 0b111000000000
#define PTE_RO 0b111111111101

struct page{
	//struct page *next;
	uint64_t base_address;
	uint16_t reference_count;
	uint64_t freed;
	uint64_t pid;
};

extern struct page pageArr[NO_OF_FREE_PAGES];
void map_all_memory_to_pageTables();
uint64_t align_4KB(uint64_t);
uint64_t cr3_content;
//uint64_t virtual_base_address = 0x0;

void start_paging(void*,void*);
uint64_t get_free_page();
void create_free_page_list(uint64_t,uint64_t);
uint64_t init_paging();

int64_t print_page_count();
void deletePageTable(uint64_t);
void enable_paging(uint64_t);
uint64_t get_physical_address(uint64_t);
void mark_physical_page_used(uint64_t);
void mark_kernel_pages_used(void*,void*);
uint64_t get_linear_address(uint64_t);
int getpml4e_index(uint64_t);
int getpdtpe_index(uint64_t);
int gettable_index(uint64_t);
int getpde_index(uint64_t);
void mapKernelMemory(void*,void*,uint64_t cr3_content);
void createPageTableEntry(uint64_t,uint64_t,uint64_t);
uint64_t create_page_block(uint64_t);
int getpml4e_index(uint64_t);
int getpdpte_index(uint64_t);
int getpte_index(uint64_t);
int getpde_index(uint64_t);
void checkLookUp(void*,void*,uint64_t);
uint64_t check_mapping(uint64_t,uint64_t);
uint64_t check_page_mapping(uint64_t,uint64_t,uint64_t);
uint64_t create_empty_page(uint64_t address,uint64_t cr3_content);
void mark_physical_page_unused(uint64_t);
void mark_page_cow(uint64_t,uint64_t,int);
uint64_t get_reference_count(uint64_t);
void disable_cow(uint64_t,uint64_t);
void enable_cow(uint64_t,uint64_t);
void free_physical_page(uint64_t,uint64_t);
void free_page(uint64_t,uint64_t);
uint64_t refresh_pte_value(uint64_t,uint64_t);
uint64_t pte_block_value(uint64_t,uint64_t);
#endif
