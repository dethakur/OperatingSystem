#include<sys/sbunix.h>
#include<sys/defs.h>
#include<sys/tarfs.h>
#include<sys/elf.h>
#include<sys/kmalloc.h>
#include<sys/paging.h>
#include<sys/util.h>
#include<sys/process.h>
#include<errno.h>



extern process* current_running_process; 


int tarfs_open(char *binname,int flags){
	int i=0,j=0,fsize,diff,fd; 
	char sizestr[12];
	//	char* srcptr;
	//	char* destptr;
	//	uint64_t ret_addr=0;
	void* file = (void*)( &(_binary_tarfs_start) );
	tarfs* filehdr = (tarfs *)file;
	char found_flag='N';
  char root_flag='N';
  
  if(strcmp("#root/",binname)==0)
     root_flag='Y';

	//current_running_process->maxfd=4; //tempo hardcoding

	while(1){
		////print("j=%d File size = %s .....File Name= %s ~~ ~~ ",j,filehdr->size,filehdr->name);


		for (i=0;i<12;i++){
			*(sizestr+i) = *((filehdr->size) + i);
		}
		fsize=convert_to_int(sizestr);

		////print("fsize=%d ;",fsize);

		if ( (strcmp(binname,filehdr->name)==0)|| root_flag=='Y'  ){


			//printf("File found!");
			if (flags!=O_RDONLY){  //tarfs cannot be opened in any mode other than rdonly
				found_flag='P'; //permission error
				break;
			}   
			current_running_process->maxfd++;
			fd=current_running_process->maxfd;
			(current_running_process->fdtable[fd]).fileptr=(void *)filehdr;
			(current_running_process->fdtable[fd]).file_offset=0;
			(current_running_process->fdtable[fd]).flags=O_RDONLY;  
			(current_running_process->fdtable[fd]).filetype='T';  //tarfs filetype
      (current_running_process->fdtable[fd]).filesize=0;
			found_flag='Y';
      
      if(root_flag=='Y'){
         (current_running_process->fdtable[fd]).filesize=-1;   //filesize -1 used to indicate root directory
         (current_running_process->fdtable[fd]).file_offset=-1;
      }
         
			break;
		}   

		diff = 512- ((sizeof(tarfs) + fsize) % 512);
		////print(";diff=%d ;",diff);
		if (diff==512)
			diff=0;
		file += sizeof(tarfs) + fsize + diff;

		if (file-(void *)&(_binary_tarfs_end) >=0){
			//			//print("\n Tarfs range exceeded!");
			break;
		}    
		else 
			filehdr=(tarfs *)(file);

		j++;
	}	

	if (found_flag=='Y'){
		//printf("fd from open=%d",fd);
		return fd;
	}else if (found_flag=='P'){
		return -1*(EACCES);   
	}else{
		//        //print("File not found!");
		return -1*(ENOENT);
	} 
}


ssize_t tarfs_read(int fd,void *buf,size_t count){

	tarfs* filehdr;
	char* srcptr;
	char* destptr;
	size_t offset,bytes_left; 
	int adj_count;
	//char sizestr[12];

	//Read fd table
	filehdr=(tarfs *)(current_running_process->fdtable[fd]).fileptr;



	// for (i=0;i<12;i++){
	//			*(sizestr+i) = *((filehdr->size) + i);
	//		}

	int fsize=convert_to_int(filehdr->size);

	//Read current offset from offset table
	offset =(current_running_process->fdtable[fd]).file_offset;
	bytes_left = fsize - offset; 

	//printf("File size = %d .....File Name= %s ~~ ~~ Offset=%d...Bytes left=%d****",fsize,filehdr->name,offset,bytes_left);

	////print("sizeof(tarfs)=%d,filehdr=%x,filehdr+sizeof(tarfs)=%x",sizeof(tarfs),filehdr,filehdr+512);

	if (bytes_left ==0)
		adj_count = 0;
	else if (bytes_left>=count)
		adj_count=count;
	else if (bytes_left < count)
		adj_count=bytes_left;    


	srcptr= ((char *)(filehdr)) + sizeof(tarfs) + offset;
	destptr = (char *) buf;

	tarfs_memcpy(srcptr,destptr,adj_count);

	(current_running_process->fdtable[fd]).file_offset += adj_count; //Increment offset by adj_count
   //printf("\n Read %d bytes",adj_count);
	return (ssize_t) adj_count;   
}      


int tarfs_close(int fd){

	//current_running_process->maxfd--;
	//Cant decrement maxfd, because fd being closed may not be the maxfd
	(current_running_process->fdtable[fd]).fileptr=0;
	(current_running_process->fdtable[fd]).file_offset=0;

	//print("File closed successfully!");
	return 0;
}


off_t tarfs_lseek(int fd,off_t offset,int whence){
  tarfs* filehdr;
 //Read fd table
	filehdr=(tarfs *)(current_running_process->fdtable[fd]).fileptr;
 
  if (!filehdr)
     return (off_t)(-1*EBADF);
  
  printf("\n Old offset of file=%d", current_running_process->fdtable[fd].file_offset ); 
   
  if (whence==SEEK_SET){
    (current_running_process->fdtable[fd]).file_offset=offset;  //set offset to value passed into function    
  }else if  (whence==SEEK_CUR){
     (current_running_process->fdtable[fd]).file_offset+=offset; //increment current offset by value passed into function
  }else if  (whence==SEEK_END){
      int fsize=convert_to_int(filehdr->size);                  //set offset to size of file + offset
      printf("\nFilesize=%d",fsize);
      (current_running_process->fdtable[fd]).file_offset=fsize+offset;
  }else{ //invalid WHENCE
     printf("\n Invalid Whence value");
     return (off_t)(-1*EINVAL);
  }     
  
  printf("\n New offset of file=%d", current_running_process->fdtable[fd].file_offset );
   return (off_t) current_running_process->fdtable[fd].file_offset ;

}


/*int strlen(char *str){

  int i;
  for(i=1;*(str)!='\0';i++,str++);
  return (i-1);
  }  
  */  
int strcmpn(char *str1,char *str2,int n){
	int i,ret=0;
	for(i=1;i<=n;str1++,str2++,i++){
		if(*str1!=*str2){
			ret=1;
			break;   
		}
		if ((*str1=='\0')||(*str2=='\0')) 
			break;
	}
	return ret;
} 

void tarfs_memcpy(char *srcptr,char *destptr,int size){
	int k;
	for (k=0;k<size;k++){
		*destptr=*srcptr;
		srcptr++;
		destptr++;
	}
}

