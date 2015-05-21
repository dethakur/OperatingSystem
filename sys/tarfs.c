#include<sys/sbunix.h>
#include<sys/defs.h>
#include<sys/tarfs.h>
#include<sys/elf.h>
#include<sys/kmalloc.h>
#include<sys/paging.h>
#include<sys/util.h>

int convert_to_int(volatile char *);
uint64_t tarfs_init(char *binname,process* p){
	int i=0,j=0,fsize,diff; 
	volatile char sizestr[12];
	Elf64_Ehdr* elfhdr;
	Elf64_Phdr* pchdr;

	volatile char* srcptr;
	volatile char* destptr;
	uint64_t ret_addr=0;
	volatile void* file = (void*)( &(_binary_tarfs_start) );
	tarfs* filehdr = (tarfs *)file;


	while(1){
		//print("j=%d File size = %s .....File Name= %s ~~ ~~ ",j,filehdr->size,filehdr->name);


		for (i=0;i<12;i++){
			*(sizestr+i) = *((filehdr->size) + i);
		}
		fsize=convert_to_int(sizestr);


		if ((fsize !=0)&& (strcmp(binname,filehdr->name)==0)){
			elfhdr=(Elf64_Ehdr *)(file + sizeof(tarfs));  
			ret_addr=(elfhdr->e_entry);
			for(i=0;i<elfhdr->e_phnum;i++){
				pchdr =(Elf64_Phdr *)(file + sizeof(tarfs) + (elfhdr->e_phoff) + i*(elfhdr->e_phentsize) );   

				if(pchdr->p_filesz ==0 && pchdr->p_memsz == 0){
					continue;
				}

				destptr=(char *)(pchdr->p_vaddr);                      //destination pointer
				srcptr=(char *) (file + sizeof(tarfs) + pchdr->p_offset );  //source pointer
				//uint64_t start_address = (uint64_t)destptr;
				//uint64_t end_address = start_address + pchdr->p_filesz;
				//uint64_t size = pchdr->p_filesz;
				allocate_VMA_memory((uint64_t)srcptr,(uint64_t)destptr,pchdr->p_filesz,pchdr->p_memsz,2,p);
				//print("src ptr %x and dest ptr = %x and fsize = %x",srcptr,destptr,size);
				//create_empty_page((uint64_t)srcptr,p);	
				//create_empty_page((uint64_t)destptr,p->reg.cr3);	
				//create_empty_page((uint64_t)destptr,p);	
				//memcpy(srcptr,destptr,(pchdr->p_filesz));
			}  
			p->start_exec = ret_addr; 
			break;

		}   

		diff = 512- ((sizeof(tarfs) + fsize) % 512);
		////print(";diff=%d ;",diff);
		if (diff==512)
			diff=0;
		file += sizeof(tarfs) + fsize + diff;

		if (file-(void *)&(_binary_tarfs_end) >=0){
			//print("\n Tarfs range exceeded!");
			break;
		}    
		else 
			filehdr=(tarfs *)(file);

		j++;
	}	
	//print(" ret_addr=%x ;", ret_addr);
	return  ret_addr;
}


int convert_to_int(volatile char *str){
	int i,res=0;
	int temp;
	//  //print("\nstr=%s",str);
	for (i=0;i<11;i++){
		temp=(int) *(str+i);      
		temp=temp-48;  //subtract 48 from ASCII value to get digit 
		////print("temp=%d;",temp);
		res=(8*res)+temp;
		////print("res=%d;",res);
	}     
	////print ("res=%d;",res);
	return res; 
}  

/*int strcmp(char *str1,char *str2){
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
}*/      



