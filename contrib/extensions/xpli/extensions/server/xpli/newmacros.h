#ifndef PLINEWMACROS_H
#define PLINEWMACROS_H
#include <sys/ioacc.h>
extern unsigned long pliSegAddr;
#define outb(port, data) BUSIO_PUTC ( ( pliSegAddr | (port)) , (data))
#define outw(port, data) BUSIO_PUTS ( ( pliSegAddr | (port)) , (data)) 
#define inb(port)  BUSIO_GETC (pliSegAddr | (port)) 
#endif /* PLINEWMACROS_H */
