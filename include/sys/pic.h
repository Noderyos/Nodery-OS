#ifndef PIC_H
#define PIC_H

#include "types.h"

struct IDT_entry {
    uint16_t offset_lower;
    uint16_t segment;
    uint8_t zero;
    uint8_t type;
    uint16_t offset_upper;
} __attribute__((packed));

struct IDT_pointer {
    uint16_t limit;
    uint32_t offset;
} __attribute__((packed));

extern void init_idt();
extern void load_idt(struct IDT_pointer *idt);
extern void enable_interrupts();

void mask_interrupts(); 
void init_idt();

#endif
