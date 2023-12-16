#include "kernel/io.h"
#include "kernel/keymap.h"
#include "asm/keyboard.h" 
#include "asm/ports.h"

u8 shift = 0;

u8 cmd[100];
u8 cmd_index = 0;

void handle_keyboard_interrupt() {
  ioport_out(PIC1_COMMAND_PORT, 0x20);

  unsigned char status = ioport_in(KEYBOARD_STATUS_PORT);
  if (status & 0x1) {
    unsigned char keycode = ioport_in(KEYBOARD_DATA_PORT);
    if(keycode & 0x80){  // Key released
      if(keycode == 0xaa || keycode == 0xb6)
        shift = 0;
    }else{ // Key pressed
      if(keycode == 0x2a || keycode == 0x36){
        shift = 1;
        return;
      }
      if(keycode == 0x0e && cmd_index > 0){
        cmd_index--;
        cmd[cmd_index] = 0;
        return;
      }

      char c;
      if(shift)
        c = keymap_maj[keycode];
      else
        c = keymap[keycode];
      if(c && cmd_index < 100){
          printChar(c);
          cmd[cmd_index] = c;
          cmd_index++;
      }
    }
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
