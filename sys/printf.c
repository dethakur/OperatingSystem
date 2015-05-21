#include<sys/sbunix.h>
#include<stdarg.h>

#define ARRAY_SIZE 
static char inputArray[160];
static int end_offset = 0 ; //This is the offset to write into the array
static int content_size = 0;

int screenLimit = 1520;
void writeTime(int,int);
void writeChar(char,int);
void writeColon(int);
void writeIntegers(int);
void writeHexadecimal(uint64_t);
char numberToHex(int);
void writeToConsole();
void write(int fd,const void* buf,uint64_t count);
void clearConsole();
void insert_blank();

/*
void scrollDown(){

	end_offset += 80;
	end_offset = end_offset > content_size ? content_size:end_offset;
	clearConsole();
	writeToConsole();
}

void scrollUp(){

	end_offset -=80;
	end_offset = end_offset < screenLimit ?(screenLimit > content_size?content_size:screenLimit):end_offset;
	clearConsole();
	writeToConsole();
}
*/

void printf(const char *format, ...) {
	va_list val;
	va_start(val,format);

	uint64_t value;	
	char* strPointer;
	char c = '0';
	while(*format) {
		if(*format == '%'){
			format++;
			switch(*format){
				case 'd':
					writeIntegers((int)va_arg(val,int));
					format++;
					break;
				case 'p':
					write(1,&c,1);
					c = 'x';
					write(1,&c,1);
					writeHexadecimal((uint64_t)va_arg(val,uint64_t));
					format++;
					break;
				case 'x':
					writeHexadecimal((uint64_t)va_arg(val,uint64_t));
					format++;
					break;
				case 'c':
					value = (uint64_t)va_arg(val,uint64_t);
					write(1,&value,1);			
					format++;
					break;
				case 's':
					strPointer = (char*)va_arg(val,char*);
					while(*strPointer){
						write(1,strPointer,1);	
						strPointer++;
					}
					format++;
					break;
				default:
					break;

			}

		}else{
			write(1,format,1);			
			format++;
		}
	}
	
/*	int empty_space = 80 - (content_size%80);
	int i=0;
	char empty_char = ' ';
	for(i=0;i<empty_space;i++){
		write(1,&empty_char,1);

	}*/
//	insert_blank();
//	writeToConsole();
//	inputArray[end_offset]=' ';
//	end_offset += 1; 
	va_end(val);	

}

void write(int fd,const void* buf,uint64_t count){
	char* c = (char*)buf;
	putchar(*c);
	return;
	char *input = (char*)buf;
	int i = 0;
	content_size += count;
	for(i=0; i< count;i++){
		inputArray[end_offset] = *input;
		end_offset += 1;
		input++;
	}
}

void clearConsole(){
	char *memory  = (char*)VIDEO_MEMORY;
	int i = 0;//start_offset;
	while(i<content_size){
		*memory = ' '; 
		memory++;
		*memory = 0x00;
		memory++;
		i++;
	}
}

void writeToConsole(){
	char *memory  = (char*)VIDEO_MEMORY;
	int i = content_size - screenLimit;
	i = i < 0 ? 0: i;
	while(i<content_size){
		*memory = inputArray[i]; 
		memory++;
		*memory = 0x07;
		memory++;
		i++;
	}
}

void writeColon(int offset){
	char *timeMemory = (char*)VIDEO_MEMORY;
	timeMemory += offset;
	*timeMemory = ':';
	timeMemory++;
	*timeMemory = 0x1F;
	timeMemory++;
}

void writeChar(char c,int offset){
	char *timeMemory = (char*)VIDEO_MEMORY;
	timeMemory += offset;
	*timeMemory = c;
	timeMemory++;
//	*timeMemory = 0x1F;
 	*timeMemory = 0x0e;  //color attribute byte
	timeMemory++;
}

void writeTime(int num,int offset){
	if(num <= 0)
		return;
	writeTime(num/10,offset-2);
	num = num %10;
	char c = '0'+num;
	char *timeMemory = (char*)VIDEO_MEMORY;
	timeMemory += offset;
	*timeMemory = c;
	timeMemory++;
	*timeMemory = 0x1F;
	timeMemory++;
}
char numberToHex(int num){
	char output = '0'+num;
	switch(num){
		case 10:
			output = 'a';
			break;
		case 11:
			output = 'b';
			break;
		case 12:
			output = 'c';
			break;
		case 13:
			output = 'd';
			break;
		case 14:
			output = 'e';
			break;
		case 15:
			output = 'f';
			break;
		default:break;	
	}
	return output;
}

void writeHexadecimal(uint64_t num){
	if(num == 0)
		return;
	writeHexadecimal(num/16);
	num = num %16;
	char c = numberToHex(num); 
	write(1,&c,1);
}
void writeIntegers(int num){
	if(num <0){
		num = num *-1;
		char c = '-';
		write(1,&c,1);
	}

	if(num == 0)
		return;
	writeIntegers(num/10);
	num = num %10;
	char c = '0'+num;
	write(1,&c,1);
}

void print(const char *format, ...) {
	va_list val;
	va_start(val,format);

	uint64_t value;	
	char* strPointer;
	char c = '0';
	while(*format) {
		if(*format == '%'){
			format++;
			switch(*format){
				case 'd':
					writeIntegers((int)va_arg(val,int));
					format++;
					break;
				case 'p':
					write(1,&c,1);
					c = 'x';
					write(1,&c,1);
					writeHexadecimal((uint64_t)va_arg(val,uint64_t));
					format++;
					break;
				case 'x':
					writeHexadecimal((uint64_t)va_arg(val,uint64_t));
					format++;
					break;
				case 'c':
					value = (uint64_t)va_arg(val,uint64_t);
					write(1,&value,1);			
					format++;
					break;
				case 's':
					strPointer = (char*)va_arg(val,char*);
					while(*strPointer){
						write(1,strPointer,1);	
						strPointer++;
					}
					format++;
					break;
				default:
					break;

			}

		}else{
			write(1,format,1);			
			format++;
		}
	}
//	insert_blank();
	//writeToConsole();
//	inputArray[end_offset]=' ';
//	end_offset += 1; 
	va_end(val);	

}
void insert_blank(){
	int sw = 80;	
	int empty_space = sw - (content_size % sw);

	int i=0;
	char empty_char = ' ';
	for(i=0;i<empty_space;i++){
		write(1,&empty_char,1);

	}
}
