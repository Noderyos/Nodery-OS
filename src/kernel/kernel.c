#include "kernel/io.h"
#include "kernel/utils.h"
#include "kernel/keymap.h"
#include "asm/keyboard.h" 
#include "asm/ports.h"
#include "asm/ata.h"
#include "types.h"

void handle_keyboard_interrupt() {
  ioport_out(PIC1_COMMAND_PORT, 0x20);

  unsigned char status = ioport_in(KEYBOARD_STATUS_PORT);
  if (status & 0x1) {
    unsigned char keycode = ioport_in(KEYBOARD_DATA_PORT);
    if (keycode < 0 || keycode >= 128) return;
    char c = keymap[keycode];
    if(c)
      printChar(c);
  }
}

extern void main(){ // The main function call in entry.asm
  init_idt();
  kb_init();
  enable_interrupts();
  //disable_cursor();
  print("Welcome to NoderyOS\nDecimal : %d\nHex : %x\nString : %s\n", 69, 0x1337, "Hello world");
  while(1);
}
