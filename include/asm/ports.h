#ifndef PORTS_H
#define PORTS_H 

#include "types.h"

extern char ioport_in(u16 port);
extern void ioport_out(u16 port, u8 data);

extern void inl(u16 port);
extern void outl(u16 port, u32 data);

#endif
