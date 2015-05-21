#include<sys/sbunix.h>
#include<sys/defs.h>
#include<sys/tarfs.h>
#include<sys/elf.h>
#include<sys/kmalloc.h>
#include<sys/paging.h>
#include<sys/process.h>
#include<sys/util.h>
#include<errno.h>

extern process* current_running_process; 


int tarfs_getdents(int fd,char *buf,int bufsize){


	// int fd=tarfs_open(dirname,0);
	// print("fd in opendir=%d",fd);
	int i=0,oset=0;
	char root_flag='N';

	//print("sizeof metadata=%d",sizeof(struct fdMetadata) );
	//struct fdMetadata *metadata = (struct fdMetadata*)(k_malloc(sizeof(struct fdMetadata)));

	//   char* tempbuf = (char*)k_malloc(NAME_MAX);  //Allocate buffer
	if(current_running_process->p_cache == 0){
		current_running_process->p_cache = (uint64_t)k_malloc(4095);
	}
	memset((char*)current_running_process->p_cache,4095,0);
	char* tempbuf = (char*)current_running_process->p_cache;
		
	//struct dirent *deptr = (struct dirent*)(k_malloc(sizeof(struct dirent)));;
	//struct fdMetadata metadata;


	//Read fd table
	tarfs *mfilehdr=(tarfs *)(current_running_process->fdtable[fd]).fileptr;
	oset=(current_running_process->fdtable[fd]).file_offset;  //Look up the offset from fd table

	if ((current_running_process->fdtable[fd]).filesize==-1){
		//printf("\n Root flag set");
		root_flag='Y';
		//print first dir
	}

	struct dirent de;
	struct dirent *deptr= (struct dirent *) buf ;



	while(1){  
		if( (bufsize-i)< sizeof(struct dirent) )   //no more room in buffer
			break;   

		oset=tarfs_read_next(fd,oset,tempbuf);  //pass current offset, and get new offset
		(current_running_process->fdtable[fd]).file_offset=oset;  //update offset on fd table to new value
		//print("\noset=%d, Buff contents=%s",oset, tempbuf);
		if (oset==-2)  //no more files to read
			break;
		//    else if ( strcmpn(tempbuf,mfilehdr->name,strlen(mfilehdr->name) ) != 0 )  //dir name does not match
		//      break;
		else if ( (strcmpn(tempbuf,mfilehdr->name,strlen(mfilehdr->name) ) == 0 ) || (root_flag=='Y') ){   //match on dir name
			//add multiple dirent structs in buffer, one for each item within the directory
			//Build a dirent structure
			de.d_ino=maxinode;
			maxinode++;

			de.d_off = oset; 
			de.d_reclen=sizeof(struct dirent);

			tarfs_memcpy(tempbuf,(char *)(de.d_name),strlen(tempbuf)+1 ); 
			//printf("de.dname=%s",(de.d_name)); 

			*deptr = de;
			deptr++;
			//i++;
			//tarfs_memcpy( (char *)&de, (char *) (base+i), sizeof(struct dirent) );

			//print("metadata.buffer=%s",( (char *) ( metadata.buffer ) ) + i );

			i+= sizeof(struct dirent);
		}

	}     

	//   metadata->fileDescriptor = fd;  
	//   metadata->offset=0;
	//   metadata->size = i;

	//   print("metadata->fileDescriptor=%d,size=%d,offset=%d,buffer= ",metadata->fileDescriptor,metadata->size,metadata->offset);


	//  print("Contents of buffer populated in getdirents:");  
	/*
	   for (j=0;j<=30;j++){
	   print("%c",buf[j]);
	   }
	   */   


	return i;
}

/*
   struct dirent *tarfs_readdir(void *dir){
   struct fdMetadata *metadata = (struct fdMetadata*)(dir);
   struct dirent* dirent = (struct dirent*)(k_malloc(sizeof(struct dirent)));

   char* buffer = metadata->buffer;
   int offset = metadata->offset;
   int size = metadata->size;
   if(offset < size && (buffer+offset)){
   dirent = (struct dirent*)(buffer + offset);
   }else{
   return NULL;

   }
   offset += dirent->d_reclen; 
   metadata->offset = offset;

   print("dirent->d_name=%s",dirent->d_name);
   return dirent;
   }
   */


int tarfs_closedir(void* dir){
	struct fdMetadata *metadata = (struct fdMetadata*)(dir);
	int fileDescriptor = metadata->fileDescriptor;
	uint64_t val = tarfs_close(fileDescriptor);
	return val;
}




//Reads the next filename (not the current one pointed to by fd, or file *)
int tarfs_read_next(int fd,int inoffset,char *buf){
	tarfs* filehdr;
	void * file;
	int fsize,diff,outoffset,offset;

	// char sizestr[12];

	//Read fd table
	file=(void *)(current_running_process->fdtable[fd]).fileptr;  //file now has the hdr for fd
	if(inoffset!=-1)
		file += inoffset;    //Increment file by inoffset 

	filehdr = (tarfs *) file; 


	//print("[Readnext]Curr File size = %s .....File Name= %s ~~ ~~ ",filehdr->size,filehdr->name);
	fsize=convert_to_int(filehdr->size);


	diff = 512- ((sizeof(tarfs) + fsize) % 512);
	//print(";diff=%d ;",diff);
	if (diff==512)
		diff=0;

	if(inoffset==-1)  //indicates root directory
		outoffset=0;    //dont skip ahead to the next file
	else{  
		offset = sizeof(tarfs) + fsize + diff;  
		file += offset;  //move file pointer to next file
		outoffset = inoffset + offset;
	}   

	if (file-(void *)&(_binary_tarfs_end) >=0){
		//			print("\n Tarfs range exceeded!");
		outoffset=-2;  //-2 used to indicate that there are no more files to read
	}    
	else{ 
		filehdr=(tarfs *)(file);
		//print("[Readnext]Next File size = %s .....File Name= %s ~~ ~~ ",filehdr->size,filehdr->name);
		tarfs_memcpy(filehdr->name,buf,100);   //100 is size of filehdr->name
	}

	return outoffset;

}


int tarfs_chdir(char *path){
	// printf("\n Entered trafs_chdir with path=%s",path);
	int ret=-2;
	//char *fullpath=(char *)k_malloc(NAME_MAX);
	if(current_running_process->p_cache == 0){
		current_running_process->p_cache = (uint64_t)k_malloc(4095);
	}
	memset((char*)current_running_process->p_cache,4095,0);
	char* cache_mem = (char*)current_running_process->p_cache;
	char *fullpath=cache_mem;

	//char *restofpath=(char *)k_malloc(NAME_MAX);
	char *restofpath=(cache_mem) + NAME_MAX + 10 ;

	//char *cwdcopy=(char *)k_malloc(NAME_MAX);
	char *cwdcopy=(cache_mem) + 2*( NAME_MAX + 10) ;

	char selectedpath[NAME_MAX]; 
	char origcwd[NAME_MAX];
	int i=0;
	char rootpath='N';

	//backup the current cwd value
	strcpy(cwd,origcwd);

	//special handling for paths beginning with ./
	//just ignore the 1st 2 characters of the path and treat the rest of the path as a relative path
	if( (*path=='.') && (*(path+1)=='/')  )
		path +=2;


	*restofpath='\0';  //initialise restofpath to empty string

	//if path begins with a / , copy it to selectedpath
	if (*path=='/'){
		selectedpath[i]=*path;
		i++;
		path++;
	}  

	//process only the first folder in the path and save the rest of the path in restofpath
	while(*path){
		selectedpath[i]=*path;
		if (selectedpath[i]=='/'){
			i++;
			strcpy(path+1,restofpath);
			break;
		}
		i++;
		path++; 
	}    
	selectedpath[i]='\0';

	//printf("\n Selectedpath=%s, restofpath=%s",selectedpath,restofpath);  


	//cwd has both leading and trailing slash   
	if(*selectedpath=='/') //absolute pathname
		strcpy(selectedpath+1,fullpath);  //path+1 skips the leading slash
	else if (  (*selectedpath=='.')  && ( *(selectedpath+1)=='.')  ){
		int cwdlen=strlen(cwd);
		strcpy(cwd,cwdcopy);
		char *cwdcopystart=cwdcopy;
		cwdcopy += (cwdlen-1); //cwdcopy is now at last position of string
		//decrement cwdcopy till a / is encountered
		for(i=1;;i++){   //i tracks the number of positions moved starting from end of sring
			cwdcopy--;
			if (*cwdcopy=='/')
				break;
		}   
		//cwdcopy is now positioned at the /
		*(cwdcopy+1)='\0';
		strcpy(cwdcopystart+1,fullpath);

	}else 
		strconcat(cwd+1,selectedpath,fullpath);  //cwd+1 skips the leading slash


	//Check if last character in fullpath is slash
	int len=strlen(fullpath);
	//printf("strlen(fullpath)=%d",strlen(fullpath));
	if(*(fullpath+len-1)!='/')
		strconcat(fullpath,"/",fullpath); //Append a / to the end of dirname 

	//printf("maxfd=%d",current_running_process->maxfd);

	//check if fullpath is the root directory
	if ( (strlen(fullpath)==1) && (*fullpath =='/') )
		rootpath='Y';


	if (strlen(fullpath)>NAME_MAX){
		ret=-1*(ENAMETOOLONG);
	}else{
		uint64_t flags=0;
		printf("\nAttempting to open fullpath=%s\n",fullpath);
		int64_t fd=tarfs_open(fullpath,flags);  //attempt to open dir
		//printf("  fd=%d",fd);
		if ( (fd>0) || (rootpath=='Y') ) {
			if (rootpath=='Y')
				strcpy("/",cwd);
			else{   
				memcpy(fullpath,cwd+1,strlen(fullpath)+1);
				tarfs_close(fd);
			}   
			if(*restofpath!='\0'){  
				//printf("\nCalling chdir recursively to process restofpath=%s",restofpath);
				ret=tarfs_chdir(restofpath); //recursively call chdir to process restofpath
				//printf("\nReturn code from child=%d",ret);
				if (ret!=0) //change cwd back to the original value
					strcpy(origcwd,cwd);
			}else   
				ret=0;
		}else
			ret=-1*(ENOENT);
	}  

	return ret; 
} 


