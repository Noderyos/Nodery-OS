#include "print.h"

#define IDT_SIZE 256
#define KERNEL_CODE_SEGMENT_OFFSET 0x8
#define IDT_INTERRUPT_GATE_32BIT 0x8e
#define PIC1_COMMAND_PORT 0x20
#define PIC1_DATA_PORT 0x21
#define PIC2_COMMAND_PORT 0xA0
#define PIC2_DATA_PORT 0xA1
#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64
extern void keyboard_handler();                                    	// Créée dans keyboard.asm
extern char ioport_in(unsigned short port);                         // Créée dans keyboard.asm
extern void ioport_out(unsigned short port, unsigned char data);    // Créée dans keyboard.asm
extern void load_idt(unsigned int* idt_address);                    // Créée dans keyboard.asm
extern void enable_interrupts();                                    // Créée dans keyboard.asm

struct IDT_pointer {
	unsigned short limit;
	unsigned int base;
} __attribute__((packed));

struct IDT_entry {
	unsigned short offset_lowerbits; 
	unsigned short selector; 
	unsigned char zero; 
	unsigned char type_attr; 
	unsigned short offset_upperbits; 
} __attribute__((packed));

struct IDT_entry IDT[IDT_SIZE]; 
int cursor_pos = 0;

void init_idt() {
	
	unsigned int offset = (unsigned int)keyboard_handler;
	IDT[0x21].offset_lowerbits = offset & 0x0000FFFF; 
	IDT[0x21].selector = KERNEL_CODE_SEGMENT_OFFSET;
	IDT[0x21].zero = 0;
	IDT[0x21].type_attr = IDT_INTERRUPT_GATE_32BIT;
	IDT[0x21].offset_upperbits = (offset & 0xFFFF0000) >> 16;

	ioport_out(PIC1_COMMAND_PORT, 0x11);
	ioport_out(PIC2_COMMAND_PORT, 0x11);
	
	ioport_out(PIC1_DATA_PORT, 0x20);
	ioport_out(PIC2_DATA_PORT, 0x28);
	
	ioport_out(PIC1_DATA_PORT, 0x0);
	ioport_out(PIC2_DATA_PORT, 0x0);
	
	ioport_out(PIC1_DATA_PORT, 0x1);
	ioport_out(PIC2_DATA_PORT, 0x1);
	
	ioport_out(PIC1_DATA_PORT, 0xff);
	ioport_out(PIC2_DATA_PORT, 0xff);
	
	struct IDT_pointer idt_ptr;
	idt_ptr.limit = (sizeof(struct IDT_entry) * IDT_SIZE) - 1;
	idt_ptr.base = (unsigned int) &IDT;
	
	load_idt(&idt_ptr);
}
void kb_init() {
	ioport_out(PIC1_DATA_PORT, 0xFD);
}
char handle_keyboard_interrupt() {
	
	ioport_out(PIC1_COMMAND_PORT, 0x20);
	unsigned char status = ioport_in(KEYBOARD_STATUS_PORT);

	if (status & 0x1) {
		char keycode = ioport_in(KEYBOARD_DATA_PORT);
		if (keycode < 0 || keycode >= 128) return; 
		return keycode;
	}
	return 166;
}