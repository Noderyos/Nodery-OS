#include "kernel/io.h"
#include "kernel/keymap.h"
#include "asm/keyboard.h" 
#include "asm/ports.h"
#include "kernel/malloc.h"
#include "fs.h"
u8 shift = 0;

char cmd[1024];
u32 cmd_index = 0;

void interpret(char* cmd);

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
      if(keycode == 0x1c){
        cmd[cmd_index] = 0;
        print(0, "\n");
        if(cmd_index > 0)
            interpret(cmd);
        for (int i = 0; i < 1024; ++i) {
          cmd[i] = 0;
        }
        cmd_index = 0;
        print(0xFFFFFF, ">");
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

void interpret(char *cmd){
    if(strncmp(cmd, "ls", 2) == 0){
        list_files();
    }else if(strncmp(cmd, "cat", 3) == 0){
        char *filename = strstr(cmd, " ") + 1;
        char *data = read_file(filename);
        if(data == 0){
          print(0xFF0000, "Cannot find '%s' on disk\n", filename);
        }else{
          print(0xFFFFFF, data);
          if(data[strlen(data) - 1] != 0xA)
              print(0x7F7F7F, "%%\n");
        }
    }else{
        print(0xFFFFFF, "Invalid command\n");
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
  print(0x7F7F7F, "Memory available : %dKb\n", available_memory / 1024);
  if(init_malloc(available_memory) < 0){
    print(0xFF0000, "[!] Malloc init error\n");
    return;
  }
  if(init_fs(0) < 0){
    print(0xFF0000, "[!] Partition init error\n");
    return;
  }
  print(0xFF7F00,"Welcome to NoderyOS\nDecimal : %d\nHex : %x\nString : %s\n", 69, 0x1337, "Hello world");

  print(0xFFFFFF, ">");
}
