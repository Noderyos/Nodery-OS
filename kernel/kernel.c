#include "sys/pic.h"
#include "sys/ps2.h"
#include "sys/ports.h"
#include "sys/keymap.h"
#include "malloc.h"
#include "sys/io.h"
#include "io.h"

#define MAX_CMD 1024

#define KB_DATA 0x60
#define KB_COMMAND 0x64

int is_shift = 0;

int cmd_len = 0;
char cmd[MAX_CMD];

void handle_keyboard() {
    u8 status = inb(KB_COMMAND);
    if (status & 1) {
        u8 keycode = inb(KB_DATA);
        if(keycode == LSHIFT) is_shift = 1;
        else if(keycode == LSHIFT+0x80) is_shift = 0;
        else if(keycode == LMAJ) is_shift = !is_shift;
        else if(keycode == BACKSPACE) {
            backspace();
            if(cmd_len) cmd[--cmd_len] = 0;    
        }
        else if(keycode == ENTER) {
            putchar('\n');
            cmd_len = 0;
            printf("Unknown command '%s' (ono)\n", cmd);
            for(int i = 0; i < MAX_CMD; i++) cmd[i] = 0;
        }
        else if (keycode < 128) {
            char c;
            if(is_shift) c = keymap_maj[keycode];
            else c = keymap[keycode];
            putchar(c);
            if(cmd_len < MAX_CMD-1) cmd[cmd_len++] = c;
        }
    }
    outb(PIC1_COMMAND, 0x20);
}

int main(void) {
    init_idt();
    kb_init();
    enable_interrupts();

    u16 low_memory = 1024;
    u16 upper_memory = *((u16*)0x802);
    u16 extended_memory = *((u16*)0x804);
    u32 available_memory = 1024*(low_memory + upper_memory + extended_memory * 64);
    printf("Memory available : %dKb\n", available_memory / 1024);
    if(init_malloc(available_memory) < 0){
        setColor(RED);
        puts("Malloc init error");
        return 0;
    }

    puts("Welcome to");
    setColor(YELLOW);
    puts("NoderyOS");
}
