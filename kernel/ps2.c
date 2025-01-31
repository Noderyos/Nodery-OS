#include "pic.h"
#include "ps2.h"
#include "ports.h"

struct IDT_entry IDT[256];

void init_idt() {
    IDT[0x21].segment = 8;
    IDT[0x21].zero = 0;
    IDT[0x21].type = 0b10001110;
    IDT[0x21].offset_lower = (u32)keyboard_handler & 0xFFFF;
    IDT[0x21].offset_upper = ((u32)keyboard_handler & 0xFFFF0000) >> 16;

    // Restart PICs
    outb(PIC1_COMMAND, 0x11); 
    outb(PIC2_COMMAND, 0x11); 

    // Start PIC1 at 0x20 and PIC2 at 0x28
    outb(PIC1_DATA, 0x20);
    outb(PIC2_DATA, 0x28);

    // Configure PIC Cascade mode
    outb(PIC1_DATA, 4);
    outb(PIC2_DATA, 2);

    // Set 8086/88 mode
    outb(PIC1_DATA, 0x1);
    outb(PIC2_DATA, 0x1);

    // Mask all interrupts
    outb(PIC1_DATA, 0xFF);
    outb(PIC2_DATA, 0xFF);

    struct IDT_pointer idt_ptr;
    idt_ptr.offset = (u32)&IDT;
    idt_ptr.limit = sizeof(struct IDT_entry)*256;
    load_idt(&idt_ptr);
}

void kb_init() {
    outb(PIC1_DATA, 0b11111101); // Unmask IRQ1
}
