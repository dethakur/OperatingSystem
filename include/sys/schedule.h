#ifndef SCHED_H
#define SCHED_H
#include<sys/process.h>
#include<sys/defs.h>
#include<sys/sbunix.h>

#define SCHED_SIZE 3000
volatile process *p_arr[SCHED_SIZE+1];
void add_to_readyQ(volatile process*);
void update_r_queue();
volatile process* get_ready_process();
volatile process* get_process_by_id();
void disp_queue();

volatile process *w_arr[SCHED_SIZE+1];
void add_to_waitQ(volatile process*);
void update_w_queue();
void timer_update();
void invalidate_r_queue(volatile uint64_t);
void invalidate_w_queue(volatile uint64_t);

volatile process *z_arr[SCHED_SIZE+1];
void add_to_zombie_queue(volatile process*);
void update_z_queue();
int is_child_zombie(volatile uint64_t);
void invalidate_z_queue(volatile uint64_t);

void child_update(volatile uint64_t);
void kb_IO_update();
void deleteFromMemory(volatile char);
void saveToMemory(volatile char);
void change_child_parents(volatile process*);
void delete_zombie(volatile uint64_t);

int is_STDIN_locked();	

#endif
