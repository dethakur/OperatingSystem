#include<sys/schedule.h>
#include<sys/paging.h>
#include<sys/keyboard.h>
#include<sys/kmalloc.h>
#include<sys/util.h>
#include<sys/pmap.h>

static int first_index = SCHED_SIZE;
static int last_index = SCHED_SIZE;

static int first_w_index = SCHED_SIZE;
static int last_w_index = SCHED_SIZE;

static int first_z_index = SCHED_SIZE;
static int last_z_index = SCHED_SIZE;

extern volatile process* current_running_process;
 extern char* keyb_buffer;
 extern int keyb_buf_index;

void add_to_readyQ(volatile process* p){
	invalidate_r_queue(p->pid);
	update_r_queue();
	if(first_index > 0)
		p_arr[first_index--] = p;
}

void update_r_queue(){
	int val = SCHED_SIZE;
	int i = 0;
	for(i=SCHED_SIZE;i>first_index;i--){
		if(p_arr[i] == NULL)
			continue;
		p_arr[val--] = p_arr[i];
	}
	last_index = SCHED_SIZE;
	first_index = val;
}

volatile process* get_process_by_id(uint64_t pid){
	volatile process* p = NULL;
	int i=0;
	for(i=SCHED_SIZE;i>first_index;i--){
		if(p_arr[i] == NULL)
			continue;

		if(p_arr[i]->pid == pid){
			p = p_arr[i];
			break;
			//print("Process invalidated %d",pid);
		}
	}
	if(p==NULL){
		for(i=SCHED_SIZE;i>first_w_index;i--){
			if(w_arr[i] == NULL)
				continue;

			if(w_arr[i]->pid == pid){
				p = w_arr[i];
				break;
			}
		}

	}
	return p;
}
void invalidate_r_queue(uint64_t pid){
	int i=0;
	for(i=SCHED_SIZE;i>first_index;i--){
		if(p_arr[i] == NULL)
			continue;

		if(p_arr[i]->pid == pid){
			p_arr[i] = NULL;
			//print("Process invalidated %d",pid);
		}
	}
	update_r_queue();
}
//Code for zombie process
void add_to_zombie_queue(volatile process* p){
	//	print("Added to zombie = %d",p->pid);
	if(first_z_index == 0){
		update_z_queue();
	}
	if(first_z_index > 0){
		z_arr[first_z_index--] = p;
	}
}

void update_z_queue(){
	int val = SCHED_SIZE;
	int i = 0;
	for(i=SCHED_SIZE;i>first_z_index;i--){
		if(z_arr[i] == NULL)
			continue;
		z_arr[val--] = z_arr[i];
	}
	last_z_index = SCHED_SIZE;
	first_z_index = val;
}

int is_child_zombie(uint64_t pid){
	int i=0;
	int val = 0;
	for(i=SCHED_SIZE;i>first_z_index;i--){
		if(z_arr[i]== NULL)
			continue;
		volatile process* p = z_arr[i];
		if(p->pid == pid){
			val = 1;
			break;
		}
	}
	return val;
}

void delete_zombie(uint64_t pid){
	int i=0;
	for(i=SCHED_SIZE;i>first_z_index;i--){
		volatile process* p = z_arr[i];
		if(p->pid == pid){
			free_process_memory(p);
			z_arr[i] = NULL;
		}
	}
	update_z_queue();
	return;
}


void invalidate_z_queue(uint64_t pid){
	int i=0;
	for(i=SCHED_SIZE;i>first_z_index;i--){
		if(z_arr[i] == NULL)
			continue;
		if(z_arr[i]->pid == pid){
			z_arr[i] = NULL;
			//print("Process invalidated %d",pid);
		}
	}
	update_z_queue();
}
//End of code for zombie
void change_child_parents(volatile process* proc){
	int i=0;
	for(i=last_index;i>first_index;i--){
		volatile process* p = p_arr[i];
		volatile process* pp = p->pp; 
		if(pp != NULL){
			if(pp->pid == proc->pid){
				p->pp = proc->pp;
				//	print("Parent process changes for %d ",p->pid);		
			}
		}
	}
}
volatile process* get_ready_process(){
	if(last_index == first_index){
		return NULL;
	}
	return p_arr[last_index--];
}

void disp_queue(){
	//	print(" ---------------------------------- \n");
	print("Pid	PPID	NAME			STATE\n");
	int i = 0;
	for(i=SCHED_SIZE;i>first_index;i--){
		if(p_arr[i] == NULL)
			continue;
		volatile process* p = p_arr[i];
		print("%d	%d	%s			RUNNING		\n",p->pid,p->ppid,p->p_name);
	}
	for(i=SCHED_SIZE;i>first_w_index;i--){
		if(w_arr[i] == NULL)
			continue;
		volatile process* p = w_arr[i];
		print("%d	%d	%s			WAITING\n",p->pid,p->ppid,p->p_name);
		//print("%d	%d	WAITING\n",p->pid,p->ppid,p->p_name);
	}
	print(" ---------------------------------- \n");
}

//Code for wait queue begins here..

void update_w_queue(){
	int val = SCHED_SIZE;
	int i = 0;
	for(i=SCHED_SIZE;i>first_w_index;i--){
		if(w_arr[i] == NULL)
			continue;
		w_arr[val--] = w_arr[i];
	}
	last_w_index = SCHED_SIZE;
	first_w_index = val;
}


void invalidate_w_queue(uint64_t pid){
	int i=0;
	for(i=SCHED_SIZE;i>first_w_index;i--){
		if(w_arr[i] == NULL)
			continue;

		if(w_arr[i]->pid == pid){
			w_arr[i] = NULL;
			//print("Process invalidated %d",pid);
		}
	}
	update_w_queue();
}

void add_to_waitQ(volatile process* p){
	if(first_w_index == 0){
		update_w_queue();
	}
	if(first_w_index > 0)
		w_arr[first_w_index--] = p;
}

void timer_update(){
	int i=0;
	for(i=SCHED_SIZE;i>first_w_index;i--){
		if(w_arr[i] == NULL)
			continue;
		volatile process* p = w_arr[i];
		if(p->w_type.type  == sleepMode){
			p->w_type.value = p->w_type.value -1;	
			if(p->w_type.value <=0){
				add_to_readyQ(p);
				w_arr[i] =NULL;
			}
		}
	}
	update_w_queue();
}
void child_update(uint64_t pid){
	int i=0;
	for(i=SCHED_SIZE;i>first_w_index;i--){
		if(w_arr[i] == NULL)
			continue;
		volatile process* p = w_arr[i];
		if(p->w_type.type == child){
			if(p->w_type.value == pid){
				delete_zombie(pid);
				//	print("Moving %d to ready queue ",p->pid);
				add_to_readyQ(p);
				w_arr[i] =NULL;
			}
		}
	}
	update_w_queue();
}
int is_STDIN_locked(){
	int found = 0;
	for(int i=SCHED_SIZE;i>first_w_index;i--){
		if(w_arr[i] == NULL)
			continue;
		volatile process* p = w_arr[i];
		int w_fd=p->w_type.value;
		//printf("\n checking for stdin locked, filetype of fd%dis %c",w_fd,p->fdtable[w_fd].filetype);
		if(p->w_type.type == file_IO && p->fdtable[w_fd].filetype=='S' &&  p->fdtable[w_fd].flags==O_RDONLY){
			found = 1;
			//printf("Process %s is waiting for a read",p->p_name);
			break;
		}
	}	
	return found;
}
void kb_IO_update(){
	int i=0;
	for(i=SCHED_SIZE;i>first_w_index;i--){
		if(w_arr[i] == NULL)
			continue;
		volatile process* p = w_arr[i];
		if(p->w_type.type == file_IO && p->w_type.value == 0){
			volatile process* old_p = current_running_process;

			current_running_process = p;
			enable_paging(p->cr3);

			keyb_buffer[keyb_buf_index++] = '\0';
			memcpy((char*)keyb_buffer,(char*)p->user_mem,keyb_buf_index);
			memset(keyb_buffer,keyb_buf_index,0);
			keyb_buf_index = 0;

			current_running_process = old_p;
			enable_paging(old_p->cr3);

			p->state = TASK_INTERRUPTIBLE;
			add_to_readyQ(p);
			w_arr[i] =NULL;
			break;
			//	}
	}
}
update_w_queue();
}

volatile process* get_waiting_process(uint64_t fd){
	int i=0;
	volatile process* proc = NULL;
	for(i=last_w_index;i>first_w_index;i--){
		volatile process* p = w_arr[i];
		if(p->w_type.type == file_IO && p->w_type.value == fd){
			proc = w_arr[i];
			break;
		}
	}
	return proc;

}

void saveToMemory(char val){
	volatile process* p = get_waiting_process(0); 
	if(p==NULL){
		return;
	}
	if(keyb_buffer == NULL){
		keyb_buffer = k_malloc(4095);
	}
	keyb_buffer[keyb_buf_index++] = val;

}



void deleteFromMemory(char val){
	volatile process* p = get_waiting_process(1); 
	if(p==NULL){
		return;
	}
	if(keyb_buf_index >0){
		keyb_buffer[keyb_buf_index--] = ' ';
	}
}


void write_kb_input(char c){
	int i=0;
	for(i=last_w_index;i>first_w_index;i--){
		if(w_arr[i] == NULL)
			continue;
		volatile process* p = w_arr[i];
		if(p->w_type.type == file_IO && p->w_type.value == 1){
			break;
		}
	}
}



