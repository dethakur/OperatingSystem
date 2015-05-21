#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <syscall.h>

void writeIntegers(int);
void writeHexadecimal(uint64_t);
char numberToHex(int);
int printf(const char *format,...) {
	va_list val;
	int printed = 0;
	va_start(val,format);
	uint64_t value;	
	char* strPointer;
	while(*format) {
		if(*format == '%'){
			format++;
			switch(*format){
				case 'd':
					writeIntegers((int)va_arg(val,int));
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
		++printed;
	}
	va_end(val);	
	return printed;
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
