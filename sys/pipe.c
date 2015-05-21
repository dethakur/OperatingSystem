#include<sys/sbunix.h>
#include<sys/defs.h>
#include<sys/tarfs.h>
#include<sys/elf.h>
#include<sys/kmalloc.h>
#include<sys/paging.h>
#include<sys/util.h>
#include<sys/process.h>
#include<sys/pipe.h>
#include<errno.h>

extern process* current_running_process; 

int create_pipe(int *pipefd){
	struct pipestruct *pipemem=(struct pipestruct *)k_malloc(sizeof(struct pipestruct));
	int fd;

	//increment maxfd by 2 so that fds for read and write ends of pipe are consecutive integers
	current_running_process->maxfd +=2;

	//set up read end of pipe
	fd=(current_running_process->maxfd)-1;
	(current_running_process->fdtable[fd]).fileptr=(void *)pipemem;
	(current_running_process->fdtable[fd]).file_offset=0;
	(current_running_process->fdtable[fd]).flags=O_RDONLY;  
	(current_running_process->fdtable[fd]).filetype='P';  //pipe filetype
	(current_running_process->fdtable[fd]).filesize=0;  
	*pipefd=fd;

	pipefd++;

	//set up write end of pipe
	fd++;
	(current_running_process->fdtable[fd]).fileptr=(void *)pipemem;
	(current_running_process->fdtable[fd]).file_offset=0;
	(current_running_process->fdtable[fd]).flags=O_WRONLY;  
	(current_running_process->fdtable[fd]).filetype='P';  //pipe filetype
	(current_running_process->fdtable[fd]).filesize=0;
	*pipefd=fd;

	pipemem->unread_bytes=0;

	return 0;
}


uint64_t pipe_read(int fd,void *buf,int64_t count){

	char* srcptr;
	char* destptr;
	//int64_t offset,bytes_left; 
	int adj_count;
	char tempbuff[DEFAULT_PIPE_SIZE];
	struct pipestruct *pipemem= (struct pipestruct *) (current_running_process->fdtable[fd].fileptr);

	if (pipemem->unread_bytes ==0)
		adj_count = 0;
	else if (pipemem->unread_bytes >=count)
		adj_count=count;
	else if (pipemem->unread_bytes < count)
		adj_count=pipemem->unread_bytes;    

	srcptr= (char *) pipemem->pipebuffer;
	destptr = (char *) buf;
	memcpy(srcptr,destptr,adj_count);
	pipemem->unread_bytes -= adj_count;

	if (adj_count !=0){
		//delete stuff that's already been read
		srcptr += adj_count;
		destptr= tempbuff;
		memcpy(srcptr,destptr,pipemem->unread_bytes);

		srcptr=tempbuff;
		destptr=(char *) pipemem->pipebuffer;
		memset(destptr,DEFAULT_PIPE_SIZE,0);
		memcpy(srcptr,destptr,pipemem->unread_bytes);
	}

	return (uint64_t) adj_count;      

}

uint64_t pipe_write(int fd,void *buf,int64_t count){

	struct pipestruct *pipemem= (struct pipestruct *) (current_running_process->fdtable[fd].fileptr);  
	char *destptr= ( (char *) pipemem->pipebuffer ) + pipemem->unread_bytes;

	if (count>DEFAULT_PIPE_SIZE-(pipemem->unread_bytes) ){
	//	printf("Data to be written exceeds pipe size. Data truncated to fit pipe");
		count=DEFAULT_PIPE_SIZE-(pipemem->unread_bytes);
	}  

	memcpy((char *)buf,  destptr , count);

	pipemem->unread_bytes += count;

	return count;
}

int setup_dup(int oldfd){

	//add code here to check if oldfd and newfd are valid
	//printf("\n Entered dup2 with oldfd=%d and ndewfd=%d",oldfd,newfd);
	current_running_process->maxfd++;
	int newfd=current_running_process->maxfd;

	(current_running_process->fdtable[newfd]).fileptr=     (current_running_process->fdtable[oldfd]).fileptr;
	(current_running_process->fdtable[newfd]).file_offset= (current_running_process->fdtable[oldfd]).file_offset;
	(current_running_process->fdtable[newfd]).flags=       (current_running_process->fdtable[oldfd]).flags;
	(current_running_process->fdtable[newfd]).filetype=    (current_running_process->fdtable[oldfd]).filetype;
	(current_running_process->fdtable[newfd]).filesize=    (current_running_process->fdtable[oldfd]).filesize;


	return newfd;
}


int setup_dup2(int oldfd,int newfd){

	//add code here to check if oldfd and newfd are valid
	//printf("\n Entered dup2 with oldfd=%d ",oldfd);


	(current_running_process->fdtable[newfd]).fileptr=     (current_running_process->fdtable[oldfd]).fileptr;
	(current_running_process->fdtable[newfd]).file_offset= (current_running_process->fdtable[oldfd]).file_offset;
	(current_running_process->fdtable[newfd]).flags=       (current_running_process->fdtable[oldfd]).flags;
	(current_running_process->fdtable[newfd]).filetype=    (current_running_process->fdtable[oldfd]).filetype;
	(current_running_process->fdtable[newfd]).filesize=    (current_running_process->fdtable[oldfd]).filesize;

	//printf("\n Newfd filetype set to %c",current_running_process->fdtable[newfd].filetype);

	return newfd;
}



