#include<sys/timer.h>
#include<sys/port.h>
#include<sys/defs.h>
#include<sys/sbunix.h>
#include<sys/process.h>
#include<sys/schedule.h>
//void writeToConsole(int);
static uint64_t i = 1;
static int seconds = 0;
static int previous_second=0;
//static int minutes = 0;
static int offset = 2800;
extern process* current_running_process;
//static int secondsPrinted = 0;
//static int scrollUpOrDown = 0;
void init_timer(){
	uint16_t frequency = 100;	
	frequency = (uint16_t)(1193180/frequency);
	outb(0x43,0x36);
	uint8_t low = (uint8_t)(frequency & 0xFF);
	uint8_t high = (uint8_t)(frequency >> 8);
	outb(0x40,low);
	outb(0x40,high);

}
void disp_timer(){
	offset = 3500;
	seconds = i/100;
	if(seconds > 0){
		if(previous_second != seconds){
			writeTime(seconds,offset);
			previous_second = seconds;
			timer_update(); 
			if(previous_second %3 ==0){

				//process *p = get_ready_process();
				//add_to_readyQ(p);
//				if(p != NULL){
//					print("Starting pid = %d",p->pid);
				//	run_process(p);	
//				}
			}else{
//				save_process_contents(current_running_process);
			}
		}
	}
	i++;	
	current_running_process->timer_val += 1;
	if(current_running_process->timer_val%2 == 0){
		current_running_process->timer_val = 0;
		prempt(0);
	}
	return;
}

