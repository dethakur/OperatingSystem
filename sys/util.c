#include<sys/util.h>
#include<sys/defs.h>
#include<sys/sbunix.h>
#include<sys/paging.h>
#include<sys/process.h>

extern process* current_running_process;
void memset(char* address, uint64_t size, uint64_t value){
	char *ptr = (char*)(address);
	int i =0;
	for(i=0;i<size;i++){
		*ptr = value;
		ptr++;
	}
}

void memcpy(char *srcptr,char *destptr,int64_t size){
	int k = 0;
	memset(destptr,size,0);
	for(k=0;k<size;k++){
		*destptr = *srcptr;


		srcptr++;
		destptr++;
	}
	//		print("[ISR] Copying data from %x to %x of size = %d\n",srcptr,destptr,size);
	//		print("[ISR] Copying data from %x to %x of k = %d\n",srcptr,destptr,k);
}


int strlen(char* bin_file){
	char* str = bin_file;
	int i =0;
	while(*str){
		i+=1;
		str++;
	}
	return i;
}

void strcpy(char *srcstr,char* deststr){
	for (;;){
		*deststr=*srcstr;
		if (*deststr=='\0')
			break;
		else{  
			deststr++;
			srcstr++;
		}
	}

}

void strconcat(char* str1,char* str2,char* str3){
	while(*str1){
		*str3=*str1;
		str1++;
		str3++;
	} 
	while(*str2){
		*str3=*str2;
		str2++;
		str3++;
	} 
	*str3='\0';
}    


uint64_t get_page_perm(uint64_t address,uint64_t cr3_content){
	//print("User Space Address = %x",address);
	uint64_t *pml4e_block = (uint64_t*)(cr3_content );
	int index = getpml4e_index(address);

	uint64_t *pdtpe_block = (uint64_t*)get_linear_address(align_4KB(pml4e_block[index]));
	index = getpdpte_index(address);

	uint64_t *pde_block = (uint64_t*)get_linear_address(align_4KB(pdtpe_block[index]));
	index = getpde_index(address);

	uint64_t *pte_block = (uint64_t*)get_linear_address(align_4KB(pde_block[index]));
	index = getpte_index(address);

	//uint64_t offset = address & 0xFFF;	
	uint64_t phys_addr = (pte_block[index]);

	uint64_t perm = phys_addr & 0xFFF;
	return perm;
}

uint64_t get_page_phys_addr(uint64_t address,uint64_t cr3_content){
	//print("User Space Address = %x",address);
	uint64_t *pml4e_block = (uint64_t*)(cr3_content );
	int index = getpml4e_index(address);

	uint64_t *pdtpe_block = (uint64_t*)get_linear_address(align_4KB(pml4e_block[index]));
	index = getpdpte_index(address);

	uint64_t *pde_block = (uint64_t*)get_linear_address(align_4KB(pdtpe_block[index]));
	index = getpde_index(address);

	uint64_t *pte_block = (uint64_t*)get_linear_address(align_4KB(pde_block[index]));
	index = getpte_index(address);

	//uint64_t offset = address & 0xFFF;	
	uint64_t phys_addr = (pte_block[index]);
	return phys_addr;	
}
void mem_check(char* s1,char* s2, int size){

	int k = 0;
	for(k=0;k<size;k++){
		//		print("[ISR] Copying data from %x to %x of size = %d",srcptr,destptr,size);
		if(*s1 != *s2){
			print("destptr = %x not equal to secptr = %x and k = %d",s1,s2,k);
			break;
		}
		s1++;
		s2++;
	}
	//	print("[ISR] Copying data from %x to %x of size = %d",srcptr,destptr,size);
	//	print("[ISR] Copying data from %x to %x of k = %d",srcptr,destptr,k);
}
void refresh_cr3(){
	process* p = current_running_process;
	enable_paging(p->cr3);
}
uint64_t stoi(char *str){
	uint64_t value = 0;
	int len = strlen(str);
	len = len -1;
	while(*str){
		int zeroVal = (int)'0';
		int strVal = (int)*str - zeroVal;
		value += strVal * (power(10,len));
		str++;	
		len --;
	}
	return value;
}
int power(int num,int power){
	int x = 1;
	while(power>0){
		x = x*num;
		power--;
	}
	return x;
}

int strcmp(char *str1,char *str2){
	int ret=0;
	for(;;str1++,str2++){
		if(*str1!=*str2){
			ret=1;
			break;   
		}
		if ((*str1=='\0')||(*str2=='\0')) 
			break;
	}
	return ret;
}      

