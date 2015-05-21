#include<sys/port.h>
#include<sys/defs.h>
void outb(uint16_t port, uint8_t val)
{
	    __asm volatile( "outb %0, %1;"
			    : : "a"(val), "Nd"(port) );
}
uint8_t inb(uint16_t port)
{
	    uint8_t ret;
	     __asm volatile( "inb %1, %0" : "=a"(ret) : "Nd"(port) );
	     return ret;
}

