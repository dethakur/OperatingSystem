#ifndef _PIPE_H
#define _PIPE_H

#include<sys/process.h>

#define DEFAULT_PIPE_SIZE 4096

int create_pipe(int *);
uint64_t pipe_read(int,void *,int64_t);
uint64_t pipe_write(int,void *,int64_t);
int setup_dup(int);
int setup_dup2(int,int);

struct pipestruct{
  int unread_bytes;
  char pipebuffer[DEFAULT_PIPE_SIZE];
}; 

#endif
