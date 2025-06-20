#ifndef PORTS_H
#define PORTS_H

#include "types.h"

#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1

u8 inb(u16 port);
void outb(u16 port, u8 value);
u16 inw(u16 port);
void outw(u16 port, u16 value);


#endif
