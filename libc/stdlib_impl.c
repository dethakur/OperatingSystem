#include <stdlib.h>
#include <syscall.h>
#include <stdio.h>
#include <stdarg.h>

void _initMemoryMap();
struct value{
	size_t value;
};
static int BUFFER_SIZE = 2048;
struct key{
	uint64_t key;
	uint64_t startAddress;
	uint64_t endAddress; 
	struct value val;
};

static struct {
	struct key kVar[4096];
	int size;
}memoryMap;


static uint64_t brkVal = 0;

int power(int,int);


int stoi(char* str);
int stoh(char* str);
/*int main2(int argc,char* argv[],char* env[]){
  _initMemoryMap();
  char *p = (char*)malloc(1000);
  char *q = (char*)malloc(1000);
  q = getcwd(p,1000);
  printf(p);
  printf(q);
  }
  */

int hexToNumber(char c){
	int zeroVal =  (int)'0';
	int output = (int)c - zeroVal;

	switch(c){
		case 'a':
			output = 10;
			break;
		case 'b':
			output = 11;
			break;
		case 'c':
			output = 12;
			break;
		case 'd':
			output = 13;
			break;
		case 'e':
			output = 14;
			break;
		case 'f':
			output = 15;
			break;
		default:
			break;
	}
	return output;
}

int stoh(char *str){
	int value = 0;
	int len = strlen(str);
	len = len -1;
	while(*str){
		int strVal = hexToNumber(*str); 
		value += strVal * (power(16,len));
		str++;	
		len --;
	}
	return value;
}
int stoi(char *str){
	int value = 0;
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

int strlen(char* input){
	int x = 0;
	while(*input){
		x++;
		input++;
	}
	return x;
}

/*
int strcmp(char* input,char *match){
	char *begin;
	begin = match;
	int matchFound = -1;
	while(input !=NULL && *input && *match){
		if(*input == *match){
			match++;
		}else{
			match = begin;
		}
		input++;	
	}
	if(!(*match)){
		matchFound = 1;
	}
	return matchFound;
}
*/

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





int scanf(const char* format,...){
	va_list val;
	va_start(val,format);
	char *input = (char*)(malloc(1000));
	read(1,input,1000);
	char *start;
	int* value;

	char* readPointer;
	readPointer = &input[0];
	char *begin;
	while(*format){
		if(*format == '%'){
			format++;
			switch(*format){
				case 's':
					start = (char*)va_arg(val,char *);
					begin = start;
					while(*readPointer != ' ' && *readPointer && *readPointer != '\n'){
						*start = *readPointer;
						start++;
						readPointer++;
					}
					while(*readPointer == ' '){
						readPointer++;
					}
					*start = '\0';
					start = begin;
					break;
				case 'd':
					value = (int*)va_arg(val,int*);
					char *temp = (char*)malloc(sizeof(input));
					begin = temp;
					while(*readPointer != ' ' && *readPointer && *readPointer != '\n'){
						*temp = *readPointer;
						temp++;
						readPointer++;
					}
					while(*readPointer == ' '){
						readPointer++;
					}
					temp = begin;
					*temp = '\0';
					*value = stoi(temp);
					free(temp);
					break;
				case 'x':
					value = (int*)va_arg(val,int*);
					char *temp2 = (char*)malloc(sizeof(input));
					begin = temp2;
					while(*readPointer != ' ' && *readPointer && *readPointer != '\n'){
						*temp2 = *readPointer;
						temp2++;
						readPointer++;
					}
					while(*readPointer == ' '){
						readPointer++;
					}
					*temp2 = '\0';
					temp2 = begin;
					*value = stoh(temp2);
					free(temp2);
					break;
				case 'c':
					start = (char*)va_arg(val,char*);
					*start = *readPointer;
					while(*readPointer == ' '){
						readPointer++;
					}
					break;
				default:	
					start = (char*)va_arg(val,char *);
					begin = start;
					while(*readPointer && *readPointer != '\n'){
						*start = *readPointer;
						start++;
						readPointer++;
					}
					*start = '\0';
					start = begin;
			}
		}else{
			format++;
		}
	}

	free(input);
	va_end(val);
	return 0;
}

off_t lseek(int fd,off_t offset,int whence){
	uint64_t output = syscall_3(SYS_lseek,fd,offset,whence);
	return output;
}

int close(int fd){
	uint64_t output = syscall_1(SYS_close,(uint64_t)fd);
	return output;
}
ssize_t write(int fd,const void *buf,size_t count){
	uint64_t output = syscall_3(SYS_write,(uint64_t)fd,(uint64_t)buf,(uint64_t)count);
	return output;
}

ssize_t read(int fd,void *buf,size_t count){
	uint64_t output = syscall_3(SYS_read,(uint64_t)fd,(uint64_t)buf,(uint64_t)count);
	return output;
}

int open(const char *pathname,int flags){
	int fd = syscall_2(SYS_open,(uint64_t)pathname,(uint64_t)flags);
	return fd;
}

int brk(void *end_data_segment){
	if(end_data_segment == 0){
		memoryMap.size = 0;
	}	

	uint64_t oldbrkVal = brkVal;
	brkVal = syscall_1(12,(uint64_t)end_data_segment);
	int returnVal = 0;
	if(oldbrkVal == brkVal)
		returnVal = -1;

	return returnVal;
}

int execve(const char* filename,char *const argv[],char *const envp[]){
	syscall_3(SYS_execve,(uint64_t)filename,(uint64_t)argv,(uint64_t)envp);
	return -1;

}

int chdir(const char* path){
	uint64_t output = syscall_1(SYS_chdir,(uint64_t)path);
	return output;
}

char *getcwd(char *buf,size_t size){
	uint64_t output = syscall_2(SYS_getcwd,(uint64_t)buf,size);
	return (char*)output;
}

void _initMemoryMap(){
	brk(0);
}

unsigned int sleep(unsigned int seconds){
	static struct timespec var;
	struct timespec rem;
	rem.seconds = 0;
	rem.nanoseconds = 0L;

	var.seconds = seconds;
	var.nanoseconds = 0L; 
	//printf("Sleeping for %d second \n",seconds);

	uint64_t val = syscall_2(SYS_nanosleep,(uint64_t)&var,(uint64_t)&rem);

	if(rem.seconds != 0){
		printf("Sleep invoked with %d seconds remaining",rem.seconds);	
	}

	//	printf("Return value froms sleep = %d \n",val);
	return val;
}

unsigned int alarm(unsigned int seconds){
	uint64_t val = syscall_1(SYS_alarm,seconds);
	return val;
}
void *malloc(size_t size){
	if(brkVal == 0)
		_initMemoryMap();

	uint64_t oldbrkVal = brkVal; //+1;

	uint64_t memoryVal = brkVal;
	memoryVal = memoryVal + size;// + 5;
	int memoryAvailable = brk((void*)memoryVal);
	if(memoryAvailable == -1){
		return 0;
	}

	struct value val;
	val.value = size;

	struct key k;
	k.key = memoryVal;
	k.startAddress = oldbrkVal;
	k.endAddress = memoryVal;
	k.val = val;

	memoryMap.kVar[memoryMap.size] = k;
	memoryMap.size += 1;
	//	printf("Memory allocated from %d to %d of  %d space\n",oldbrkVal,memoryVal,size);
	return (void*)oldbrkVal;
}

void free(void *ptr){
	int length = memoryMap.size -1;
	int i;
	uint64_t key = (uint64_t)ptr;
	for(i=0;i<=length;i++){
		if(memoryMap.kVar[i].key == -1)
			continue;
		uint64_t endVal = memoryMap.kVar[i].endAddress;
		uint64_t startVal = memoryMap.kVar[i].startAddress;
		if(key >= startVal && key <= endVal ){

			uint64_t value = (uint64_t)memoryMap.kVar[i].val.value;			
			uint64_t address = brkVal - value;
			//			printf("Memory freed for = %d starting at %d and ending at %d \n",value,startVal,endVal);
			memoryMap.kVar[i].key = -1;
			memoryMap.kVar[i].val.value = -1;

			brk((void*)address);
			break;

		}
	}
	return;
}

void exit(int status){
	syscall_1(60,status);
	return;
}

pid_t fork(void){
	pid_t cpid = getpid();
	pid_t new_id = (pid_t)syscall_0(57);
	if(cpid == getppid()){
		new_id = 0;
	}
	return new_id;
}

pid_t waitpid(pid_t pid, int *status, int options){
	pid_t val = syscall_3(SYS_wait4,(uint64_t)pid,(uint64_t)status,(uint64_t)options);
	return val;
}

pid_t getpid(void){
	pid_t process_id;
	process_id =  (pid_t)syscall_0(39);
	return process_id;
}
pid_t getppid(void){
	pid_t process_id;
	process_id = (pid_t)syscall_0(110);
	return process_id;
}
int pipe(int filedes[2]){
	int val = syscall_1(SYS_pipe,(uint64_t)filedes);
	return val;
}
int dup(int oldfd){
	int val = syscall_1(SYS_dup,(uint64_t)oldfd);
	return val;

}
int dup2(int oldfd,int newfd){
	int val = syscall_2(SYS_dup2,(uint64_t)oldfd,(uint64_t)newfd);
	return val;
}

void *opendir(const char *name){
	struct fdMetadata *metadata = (struct fdMetadata*)(malloc(sizeof(struct fdMetadata)));
	//int64_t fileDescriptor = open(name,O_DIRECTORY); 
	int64_t fileDescriptor = open(name,O_RDONLY);
	if (fileDescriptor<0){
		printf("File does not exist");
		metadata=NULL;
	}else{  
		metadata->fileDescriptor = fileDescriptor;
		int val = syscall_3(SYS_getdents,(uint64_t)fileDescriptor,(uint64_t)metadata->buffer,(uint64_t)BUFFER_SIZE);
		if(val == -1)
			return NULL;
		metadata->size = val;
		metadata->offset = 0;
	}
	return (void*)metadata;  
}

struct dirent *readdir(void *dir){
	struct fdMetadata *metadata = (struct fdMetadata*)(dir);
	struct dirent* dirent = (struct dirent*)(malloc(sizeof(struct dirent)));
	//printf("sizeof(struct dirent)=%d",sizeof(struct dirent));
	char* buffer = metadata->buffer;
	int offset = metadata->offset;
	int size = metadata->size;
	//printf("offset=%d,size=%d",offset,size);

	if(offset < size && (buffer+offset) && ((offset+sizeof(struct dirent))<=DIR_BUFFER_SIZE)   ){
		dirent = (struct dirent*)(buffer + offset);
		//  	offset += dirent->d_reclen; 
		offset += sizeof(struct dirent);
		metadata->offset = offset;
	}else if ((offset+sizeof(struct dirent))>DIR_BUFFER_SIZE){
		//    printf("\n Possibly more dirents, need to call getdents()"); 
		int val = syscall_3(SYS_getdents,(uint64_t)metadata->fileDescriptor,(uint64_t)metadata->buffer,(uint64_t)BUFFER_SIZE);
		if (val<=0)
			dirent=NULL;
		else{   //val is greater than 0, so there must be atleast one dirent entry in the buffer
			metadata->size = val;
			dirent = (struct dirent*)(metadata->buffer);  //return the 1st dirent ptr at the start of the buffer
			metadata->offset = sizeof(struct dirent);     //set offset to size of  1 dirent
		}       
	} else 
		dirent=NULL;
	//	printf("Ofset value = %d\n",offset);
	//	printf("D Name = %d\n",dirent->d_name);
	//	printf("Ofset value = %d\n",dirent->d_off);
	return dirent;
}
int closedir(void* dir){
	struct fdMetadata *metadata = (struct fdMetadata*)(dir);
	int fileDescriptor = metadata->fileDescriptor;
	uint64_t val = close(fileDescriptor);
	return val;
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



