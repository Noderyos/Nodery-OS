#include "kernel/io.h"
#include "kernel/utils.h"
#include "kernel/keymap.h"
#include "kernel/keyboard.h" 

#define COMMAND_BUFFER_SIZE 100

struct IDT_entry IDT[IDT_SIZE];

char command_buffer[COMMAND_BUFFER_SIZE];
int command_len = 0;

void disable_cursor() {
  ioport_out(0x3D4, 0x0A);
  ioport_out(0x3D5, 0x20);
}
void init_idt() {
  // Address of the function keyboard_handler
  unsigned int kb_handler_offset = (unsigned long)keyboard_handler;

  // Interrupt 0x21 is the first available interupt
  IDT[0x21].offset_lowerbits = kb_handler_offset & 0x0000FFFF;
  IDT[0x21].selector = KERNEL_CODE_SEGMENT_OFFSET;
  IDT[0x21].zero = 0;
  IDT[0x21].type_attr = IDT_INTERRUPT_GATE_32BIT;
  IDT[0x21].offset_upperbits = (kb_handler_offset & 0xFFFF0000) >> 16;

  // ======= Init the PIC ======

  // Restart PIC
  ioport_out(PIC1_COMMAND_PORT, 0x11);
  ioport_out(PIC2_COMMAND_PORT, 0x11);
  
  // Start PIC1 at value 32 and PIC2 at value 40
  ioport_out(PIC1_DATA_PORT, 0x20);
  ioport_out(PIC2_DATA_PORT, 0x28);
  
  // I don't know what i done here, but it works, so ....
  ioport_out(PIC1_DATA_PORT, 0x0);
  ioport_out(PIC2_DATA_PORT, 0x0);
  
  // Set mode (8086/8088 mode)
  ioport_out(PIC1_DATA_PORT, 0x1);
  ioport_out(PIC2_DATA_PORT, 0x1);
  
  // Mask all interupts, why ? idk, the osdev says it
  ioport_out(PIC1_DATA_PORT, 0xff);
  ioport_out(PIC2_DATA_PORT, 0xff);


  struct IDT_pointer idt_ptr;
  idt_ptr.limit = (sizeof(struct IDT_entry) * IDT_SIZE) - 1;
  idt_ptr.base = (unsigned long) &IDT;

  load_idt(&idt_ptr);
}

void kb_init() {
  ioport_out(PIC1_DATA_PORT, 0xFD);
}

void handle_keyboard_interrupt() {
  ioport_out(PIC1_COMMAND_PORT, 0x20);

  unsigned char status = ioport_in(KEYBOARD_STATUS_PORT);
  if (status & 0x1) {
    unsigned char keycode = ioport_in(KEYBOARD_DATA_PORT);
    if (keycode < 0 || keycode >= 128) return;
    char c = keymap[keycode];
    if(c)
      printChar(c);
    else
     int2hex(keycode);

  }
}

extern void main(){ // The main function call in entry.asm
  init_idt();
  kb_init();
  enable_interrupts();
  //disable_cursor();

  printString("Welcome to NoderyOS\n");

  while(1);
}
