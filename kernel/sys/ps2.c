#include "sys/pic.h"
#include "sys/ps2.h"
#include "sys/ports.h"
#include "io.h"

struct IDT_entry IDT[256];

void int_handle() {
    puts("\n\nOH NO AN EXCEPTION\n\n");
    while (1);
}

void init_idt() {
    IDT[0x20].segment = 8;
    IDT[0x20].zero = 0;
    IDT[0x20].type = 0b10001110;
    IDT[0x20].offset_lower = (u32)tick_handler & 0xFFFF;
    IDT[0x20].offset_upper = ((u32)tick_handler & 0xFFFF0000) >> 16;

    IDT[0x21].segment = 8;
    IDT[0x21].zero = 0;
    IDT[0x21].type = 0b10001110;
    IDT[0x21].offset_lower = (u32)keyboard_handler & 0xFFFF;
    IDT[0x21].offset_upper = ((u32)keyboard_handler & 0xFFFF0000) >> 16;

    IDT[0x2C].segment = 8;
    IDT[0x2C].zero = 0;
    IDT[0x2C].type = 0b10001110;
    IDT[0x2C].offset_lower = (u32)mouse_handler & 0xFFFF;
    IDT[0x2C].offset_upper = ((u32)mouse_handler & 0xFFFF0000) >> 16;


    for(int i = 0; i < 0x16; i++) {
        IDT[i].segment = 8;
        IDT[i].zero = 0;
        IDT[i].type = 0b10001110;
        IDT[i].offset_lower = (u32)int_handler & 0xFFFF;
        IDT[i].offset_upper = ((u32)int_handler & 0xFFFF0000) >> 16;
    }

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

void init_pit(u16 divisor) {
    outb(0x43, 0b00110110);
    outb(0x40, divisor & 0xFF);
    outb(0x40, divisor >> 8);
} 

void kb_init() {
}

void mask_interrupts() {
    outb(PIC1_DATA, 0b11111000); // Unmask IRQ0, IRQ1 & IRQ2
    outb(PIC2_DATA, 0b11101111); // Unmask IRQ4 
}

void mouse_init() {
  
    // Enable second PS/2
    outb(PS2_COMMAND, 0xA8);

    outb(PS2_COMMAND, 0x20);
    u8 status = inb(PS2_DATA);
    status |= 0x2;
    outb(PS2_COMMAND, 0x60);
    outb(PS2_DATA, status);

    outb(PS2_COMMAND, 0xD4);
    outb(PS2_DATA, 0xF4);
}
