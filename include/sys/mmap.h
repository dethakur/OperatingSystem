#include <sys/defs.h>
#include <sys/sbunix.h>

typedef enum {R,W,RW} user_perm; 

struct vm_area_struct{
	uint64_t start_address;
	uint64_t end_address;
	int file_backed;
	struct vm_area_struct *next;
	user_perm permission;
};

typedef struct vm_area_struct vm_area_struct; 
struct mm_struct{
	uint64_t stack; //Stores top to down. Allocate properly
	uint64_t mmap_base; //Stores top to down. Allocate propoerly
	uint64_t heap;
	uint64_t start_code,end_code;
	uint64_t start_data,end_data;
	vm_area_struct *vm_map;
};
typedef struct mm_struct mm_struct;
