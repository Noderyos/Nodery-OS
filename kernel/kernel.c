#include "sys/pic.h"
#include "sys/ps2.h"
#include "sys/ports.h"
#include "sys/keymap.h"
#include "malloc.h"
#include "sys/io.h"
#include "io.h"
#include "sys/lba.h"
#include "mbr.h"
#include "fat.h"
#include "string.h"
#include "sys/vga.h"

struct mbr *mbr = (void *)0x7c00;

#define MAX_CMD 1024

int is_shift = 0;

int cmd_len = 0;
char cmd[MAX_CMD];

void handle_keyboard() {
    u8 status = inb(PS2_COMMAND);
    if (status & 1) {
        u8 keycode = inb(PS2_DATA);
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

u8 mouse_packet[3];
u8 mouse_cycle = 0;

u32 mouse_x = 0;
u32 mouse_y = 0;

void handle_mouse() {
    outb(PIC2_COMMAND, 0x20);
    outb(PIC1_COMMAND, 0x20);

    u8 status = inb(PS2_COMMAND);
    if (status & 1) {
        u8 data = inb(PS2_DATA);
        mouse_packet[mouse_cycle++] = data;
        if (mouse_cycle == 3) {
            mouse_cycle = 0;
            u32 x = mouse_packet[1] - ((mouse_packet[0]>>4)&1)*256;
            u32 y = mouse_packet[2] - ((mouse_packet[0]>>5)&1)*256;

            mouse_x += x;
            mouse_y -= y;

            if (mouse_x < 0) mouse_x = 0;
            if (mouse_y < 0) mouse_y = 0;
            if (mouse_x >= 800) mouse_x = 799;
            if (mouse_y >= 600) mouse_y = 599;

            putpixel(mouse_x, mouse_y, WHITE);            
        }
    }

}

extern void putpixel(u16 x, u16 y, u32 color);

int main(void) {
    init_idt();
    kb_init();
    mouse_init();
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

    if(initSerial(coms[0])) {
        printf("ERROR : Failed to initialize serial port");
    }

    puts("Welcome to");
    setColor(YELLOW);
    puts("NoderyOS");
    setColor(WHITE);

    if (init_fs(mbr->parts[0].lba_start) < 0) {
        setColor(RED);
        printf("Cannot init FS\n");
        return 0;
    }

    FILE *f = fopen("HELLO.TXT", "r");
    if (!f) {
        printf("Cannot open file\n");
        return 0;
    }

    char buf[32];
    u32 i = fread(buf, 1, 32, f);
    printf("%d '%s'\n", i, buf);
    memset(buf, 0, 32);
    fseek(f, 0, SEEK_SET);
    
    char *nod = "Noderyos";
    fwrite(nod, 1, strlen(nod), f);
    fseek(f, 0, SEEK_SET);
    i = fread(buf, 1, 32, f);
    printf("%d '%s'\n", i, buf);
}
