#include "kernel/io.h"
#include "kernel/keymap.h"
#include "asm/keyboard.h" 
#include "asm/ports.h"
#include "kernel/malloc.h"
#include "vga.h"
u8 shift = 0;

u8 cmd[1024];
u32 cmd_index = 0;

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
      if(c && cmd_index < 1024){
          printChar(0x00E000, c);
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
  u16 low_memory = 1024;                   // Assuming computer have at least 1Mb of RAM (Without 1Mb, BIOS isn't stable)
  u16 upper_memory = *((u16*)0x800);       // kB between 1Mb and 16Mb
  u16 extended_memory = *((u16*)0x802);    // 64kB block over 16Mb
  u32 available_memory = 1024*(low_memory + upper_memory + extended_memory * 64);
  print(0xFFFFFF, "Memory available : %dKb\n", available_memory / 1024);
  if(init_malloc(available_memory) < 0){
      print(0xFF0000, "[!] Malloc init error\n");
      goto loop;
  }

  print(0xFFFFFF,"Welcome to NoderyOS\nDecimal : %d\nHex : %x\nString : %s\n", 69, 0x1337, "Hello world");
loop:
  goto loop;
}
