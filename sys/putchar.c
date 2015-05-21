#include<sys/sbunix.h>
#include<stdarg.h>
#include<sys/util.h>
#define PUTCHAR_BUFFER_SIZE 10000
#define BUFFER_REFRESH_LINE_COUNT 42  //buffer refresh trigerred after these many lines are output
#define BUFFER_REFRESH_DELETED_LINES 15  //these many lines are deleted from the top of the buffer during a buffer refresh 

static char printbuffer[PUTCHAR_BUFFER_SIZE];
static int buffer_ind=0;
static int screen_ind=0;
static int first_visible_ind=0;        //The character at this buffer index is **ALWAYS** displayed on line 1, position 1 of the screen.
static int saved_first_visible_ind=0;  //This saves the value of first_visible_ind at the end of the last write call. This is required because 
//random user scrolling changes the value of first_visible_ind
void screen_refresh(int);
void buffer_refresh(void);

void putchar(char c){
	int i;
	int blank_count;

	char *screen  = (char*)VIDEO_MEMORY;


	if (buffer_ind==0){ //first call to this function
		for(i=0;i<PUTCHAR_BUFFER_SIZE;i++){
			printbuffer[i]=0x00;  //init print buffer
		}    
	}

	//restore screen to the state it was in at the end of the previous putchar call (undo effects of user scrolling)
	if(first_visible_ind!=saved_first_visible_ind){
		first_visible_ind=saved_first_visible_ind;
		screen_refresh(0);  //first_visible_ind already set above, so no increment/decrement required, so passing 0 to screen_refresh
	}  

	if (buffer_ind == BUFFER_REFRESH_LINE_COUNT*160){
		//printf("Starting buffer refresh");
		//printf("Before: buffer[0]=%d,buffer[2]=%d",printbuffer[0],printbuffer[2]);
		buffer_refresh();
		//printf("After: buffer[0]=%d,buffer[2]=%d",printbuffer[0],printbuffer[2]);
		screen_refresh(-160*BUFFER_REFRESH_DELETED_LINES);  //first buffer index visible decremented  to account for change in buffer data after buffer refresh
	}


	//if ((buffer_ind>=3360)&&(buffer_ind%160==0)){ //one full screen written
	if (screen_ind == 3360){ //screen is full
		//printf("Screenref!");
		screen_refresh(160);  //first buffer index visible skips ahead by 160 poistions (1 line)
		screen_ind = 3200;  //position screen_ind at beginning of last line
	}  

	if (c=='\n'){
		blank_count=160-((buffer_ind)%160); 
		//printf("blank_count=%d",blank_count);

		for(i=1;i<=blank_count;i++){

			printbuffer[buffer_ind]=0x00;
			buffer_ind++;


			screen[screen_ind]=0x00;
			screen_ind ++ ;
		}




		/*   
		     for(i=1;i<=blank_count;i++){
		     buffer_ind++;
		     printbuffer[buffer_ind]=' ';

		     screenptr += buffer_ind ;
		 *screenptr=' ';

		 buffer_ind++;
		 printbuffer[buffer_ind]=0x00;

		 screenptr++;
		 *screenptr=0x00;
		 }
		 */   
	}else{   

		//printf("Cpos=%d ",buffer_ind);
		printbuffer[buffer_ind]=c;
		buffer_ind++;

		screen[screen_ind]=c;
		screen_ind++ ;



		//printf("Cpos=%d ",buffer_ind);
		printbuffer[buffer_ind]=0x0e;
		buffer_ind++;


		screen[screen_ind]=0x0e;
		screen_ind++ ;
	}


	saved_first_visible_ind=first_visible_ind; 

	/*
	 *screenptr='*';
	 screenptr++;
	 *screenptr=0x0e;
	 screenptr++;
	 if (c=='~')
	//printf("Cpos=%d",buffer_ind);
	;
	*/
}



void screen_refresh(int skip){
	int i;
	char *screenptr  = (char*)VIDEO_MEMORY;

	first_visible_ind += skip;

	//Populate all lines 1-21 of the screen by reading characters sequentially from the printbuffer starting from index first_visible_ind
	//21 lines * 80 chars per line * 2 bytes per char gives 3360
	//Line 22 of the screen is reserved for timer, and not used by putchar 
	for(i=0;i<3360;i++){
		*(screenptr+i)=printbuffer[first_visible_ind+i];  
	}

}



void buffer_refresh(void){
	//a buffer refresh is trigerred when the buffer has 7040 characters (44 lines)
	//this function gets rid of the first 1600 characters (10 lines), and adjusts the buffer so that 
	//characters are left-shifted by 1600 positions , creating more free space at the end of the buffer

	char tempbuffer[PUTCHAR_BUFFER_SIZE];
	int delta=(160*BUFFER_REFRESH_DELETED_LINES);
	memcpy(printbuffer+delta,tempbuffer,PUTCHAR_BUFFER_SIZE-delta);
	memset(printbuffer,PUTCHAR_BUFFER_SIZE,0);
	memcpy(tempbuffer,printbuffer,PUTCHAR_BUFFER_SIZE-delta);
	buffer_ind -= delta;


}



void scrollUp(){
	if (first_visible_ind>=160)
		screen_refresh(-160);
	//this just decrements the value of first_visible_ind by 160 so that one extra line at the top is displayed. 
	//the whole screen is refreshed in sync with this change.    
}



void scrollDown(){
	if (first_visible_ind<=buffer_ind-(buffer_ind%160)-(21*160))
		screen_refresh(160);
}



