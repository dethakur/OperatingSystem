#include<sys/defs.h>

enum { O_RDONLY = 0, O_WRONLY = 1, O_RDWR = 2, O_CREAT = 0x40, O_DIRECTORY = 0x10000 };
void memset(char*,uint64_t,uint64_t);
void memcpy(char*, char*,int64_t);
uint64_t get_page_perm(uint64_t,uint64_t);
uint64_t get_page_phys_addr(uint64_t ,uint64_t );
void mem_check(char*,char*,int);
void refresh_cr3();
int strlen(char*);
void strcpy(char*,char*);
void strconcat(char* ,char* ,char* );
uint64_t stoi(char*);
int power(int,int);
int strcmp(char*,char*);
