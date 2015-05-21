#ifndef __SBUNIX_H
#define __SBUNIX_H

#include <sys/defs.h>
#define VIDEO_MEMORY 0xFFFFFFFF800B8000
void printf(const char *fmt, ...);
void print(const char *fmt, ...);
void writeTime(int,int);
void writeColon(int);
void scrollDown();
void scrollUp();
void putchar(char);

#endif
