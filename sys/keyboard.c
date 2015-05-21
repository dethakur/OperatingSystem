#include<sys/keyboard.h>
#include<sys/port.h>
#include<sys/defs.h>
#include<sys/sbunix.h>
#include<sys/schedule.h>


short int kb_flag=0;
short int shift_mode=0;
short int ctrl_mode=0;
char tempchar=' ';
char* keyb_buffer= NULL;
uint64_t keyb_buf_index;

char scancode_to_char(int scancode){
	char output = ' '; 
	kb_flag=0;    
	switch(scancode){
		case 0x2a:                                          //left shift pressed
		case 0x36:   shift_mode=1;  kb_flag = -1;   break;  //right shift pressed

		case 0xaa:                                          //left shift released
		case 0xb6:   shift_mode=0;  kb_flag = -1;   break;  //right shift released


		case 0x1d:   ctrl_mode=1;   kb_flag=-1;    break;  // ctrl pressed


		case 0x9d:   ctrl_mode=0;   kb_flag=-1;    break;  // ctrl released

		case 0x02:   output = (shift_mode ==1) ? '!' :'1'; break;
		case 0x03:   output = (shift_mode ==1) ? '@' :'2'; break;
		case 0x04:   output = (shift_mode ==1) ? '#' :'3'; break;
		case 0x05:   output = (shift_mode ==1) ? '$' :'4'; break;
		case 0x06:   output = (shift_mode ==1) ? '%' :'5'; break;
		case 0x07:   output = (shift_mode ==1) ? '^' :'6'; break;
		case 0x08:   output = (shift_mode ==1) ? '&' :'7'; break;
		case 0x09:   output = (shift_mode ==1) ? '*' :'8'; break;
		case 0x0a:   output = (shift_mode ==1) ? '(' :'9'; break;
		case 0x0b:   output = (shift_mode ==1) ? ')' :'0'; break;
		case 0x0c:   output = (shift_mode ==1) ? '_' :'-'; break;  
		case 0x0d:   output = (shift_mode ==1) ? '+' :'='; break;  
		case 0x0e:   output = (shift_mode ==1) ? 0x08 :0x08; kb_flag = -1; break;  //bkspace 
			     //       case 0x0f:   output = (shift_mode ==1) ? 'S' :'s'; break;  //tab

		case 0x10:   output = (shift_mode ==1) ? 'Q' :'q'; break;
		case 0x11:   output = (shift_mode ==1) ? 'W' :'w'; break;
		case 0x12:   output = (shift_mode ==1) ? 'E' :'e'; break;
		case 0x13:   output = (shift_mode ==1) ? 'R' :'r'; break;
		case 0x14:   output = (shift_mode ==1) ? 'T' :'t'; break;
		case 0x15:   output = (shift_mode ==1) ? 'Y' :'y'; break;
		case 0x16:   output = (shift_mode ==1) ? 'U' :'u'; break;
		case 0x17:   output = (shift_mode ==1) ? 'I' :'i'; break;
		case 0x18:   output = (shift_mode ==1) ? 'O' :'o'; break;
		case 0x19:   output = (shift_mode ==1) ? 'P' :'p'; break;
		case 0x1a:   output = (shift_mode ==1) ? '{' :'['; break;
		case 0x1b:   output = (shift_mode ==1) ? '}' :']'; break;
		case 0x1c:   output = (shift_mode ==1) ? 0x0d :0x0d; kb_flag = -1; break;  //ENTER KEY
			     //     case 0x1d:   output = (shift_mode ==1) ? ' ' :' '; break;  //left ctrl
		case 0x1e:   output = (shift_mode ==1) ? 'A' :'a'; break;
		case 0x1f:   output = (shift_mode ==1) ? 'S' :'s'; break;

		case 0x20:   output = (shift_mode ==1) ? 'D' :'d'; break;
		case 0x21:   output = (shift_mode ==1) ? 'F' :'f'; break;
		case 0x22:   output = (shift_mode ==1) ? 'G' :'g'; break;

		case 0x23:   output = (shift_mode ==1) ? 'H' :'h'; 
			     if (ctrl_mode==1){ 
				     output=0x08;  //backspace character 
				     kb_flag=-1;
			     }
			     break;   


		case 0x24:   output = (shift_mode ==1) ? 'J' :'j'; break;
		case 0x25:   output = (shift_mode ==1) ? 'K' :'k'; break;
		case 0x26:   output = (shift_mode ==1) ? 'L' :'l'; break;
		case 0x27:   output = (shift_mode ==1) ? ':' :';'; break;
		case 0x28:   output = (shift_mode ==1) ? 0x22 : 0x27 ; break;   //0x22, 0x27 : ascii codes for " and ' respectively
		case 0x29:   output = (shift_mode ==1) ? '~' :'`'; break;
			     //   case 0x2a:   output = (shift_mode ==1) ? ' ' :' '; break;      //left shift pressed
		case 0x2b:   output = (shift_mode ==1) ? '|' :0x5c ; break;    //0x5c: ascii code for backslash cgarcter
		case 0x2c:   output = (shift_mode ==1) ? 'Z' :'z'; break;  
		case 0x2d:   output = (shift_mode ==1) ? 'X' :'x'; break;  
		case 0x2e:   output = (shift_mode ==1) ? 'C' :'c'; break;
		case 0x2f:   output = (shift_mode ==1) ? 'V' :'v'; break;


		case 0x30:   output = (shift_mode ==1) ? 'B' :'b'; break;
		case 0x31:   output = (shift_mode ==1) ? 'N' :'n'; break;
		case 0x32:   output = (shift_mode ==1) ? 'M' :'m'; 
			     if (ctrl_mode==1){ 
				     output=0x0d;  //carriage return  character 
				     kb_flag=-1;
			     }
			     break;   

		case 0x33:   output = (shift_mode ==1) ? '<' :','; break;
		case 0x34:   output = (shift_mode ==1) ? '>' :'.'; break;
		case 0x35:   output = (shift_mode ==1) ? '?' :'/'; break;
			     //   case 0x36:   output = (shift_mode ==1) ? ' ' :' '; break;  //right shift pressed
			     //   case 0x37:   output = (shift_mode ==1) ? ':' :';'; break;
			     //   case 0x38:   output = (shift_mode ==1) ? '"' :"'"; break;
		case 0x39:   output = (shift_mode ==1) ? ' ' :' '; break;  //space

		case 0x48:  output = 0x11  ; kb_flag=-1;   break;  //up arrow
		case 0x50:  output = 0x12  ; kb_flag=-1;   break;  //down arrow


		default:   kb_flag=-1;     break;   //unprintable character
	}  
	return output;
}    


void display_char(){

	uint8_t kb_data;
	uint8_t kb_status;
	int offset = 3480;

	while (  (kb_status=inb(0x64) ) &  0x01  ) {

		kb_data=inb(0x60);
		//   printf(" Scan code = %d",kb_data);

		char pressed_key=scancode_to_char(kb_data);

		if (pressed_key == 0x08){  //backspace
			tempchar=0x5c;         //backslash
			writeChar(tempchar,offset);
			deleteFromMemory('b');
			//printf("%c",tempchar);
			tempchar='b';
			writeChar(tempchar,offset+2);
			//printf("%c",tempchar);
		}
		else if (pressed_key == 0x0d){  //enter key
			//     printf("\n Enter key detected!");
			tempchar=0x5c;         //backslash
			writeChar(tempchar,offset);
			//  printf("%c",tempchar);
			tempchar='n';
			kb_IO_update();
			writeChar(tempchar,offset+2);
			//printf("%c",tempchar);
			putchar('\n');
		}
		else if (pressed_key == 0x11){  //up arrow key
			//     printf("Entered up");
			scrollUp();
			tempchar=0x5c;         //backslash
			writeChar(tempchar,offset);
			//printf("%c",tempchar);
			tempchar='u';
			writeChar(tempchar,offset+2);
			//  printf("%c",tempchar);
		}
		else if (pressed_key == 0x12){  //down arrow key
			//     printf("Entered down");
			scrollDown();
			tempchar=0x5c;         //backslash
			writeChar(tempchar,offset);
			// printf("%c",tempchar);
			tempchar='d';
			writeChar(tempchar,offset+2);
			// printf("%c",tempchar);
		}
		else if (kb_flag ==0){     //printable characters only
			writeChar(pressed_key,offset);
			//printf("%c",pressed_key);
			putchar(pressed_key);
			saveToMemory(pressed_key);
			if (tempchar != ' '){
				tempchar =' ';
				writeChar(tempchar,offset+2);
			}
		}      

	}    

	return;
}

