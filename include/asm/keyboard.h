#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "types.h"
#include "asm/ports.h"

#define IDT_SIZE 256
#define KERNEL_CODE_SEGMENT_OFFSET 0x8
#define IDT_INTERRUPT_GATE_32BIT 0x8e
#define PIC1_COMMAND_PORT 0x20
#define PIC1_DATA_PORT 0x21
#define PIC2_COMMAND_PORT 0xA0
#define PIC2_DATA_PORT 0xA1
#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64


struct IDT_pointer {
    u16 limit;
    u32 base;
} __attribute__((packed));

struct IDT_entry {
    u16 offset_lowerbits;
    u16 selector;
    u8 zero;
    u8 type_attr;
    u16 offset_upperbits;
} __attribute__((packed));

extern void load_gdt();
extern void keyboard_handler();
extern void load_idt(struct IDT_pointer* idt_address);
extern void enable_interrupts();


void init_idt();
void kb_init();
void handle_keyboard_interrupt();

#endif
