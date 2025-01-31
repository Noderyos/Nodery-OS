#ifndef PIC_H
#define PIC_H

#include "types.h"

struct IDT_entry {
    u16 offset_lower;
    u16 segment;
    u8 zero;
    u8 type;
    u16 offset_upper;
} __attribute__((packed));

struct IDT_pointer {
    u16 limit;
    u32 offset;
} __attribute__((packed));

extern void init_idt();
extern void load_idt(struct IDT_pointer *idt);
extern void enable_interrupts();

void init_idt();

#endif
