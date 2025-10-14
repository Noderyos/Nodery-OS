#include "sys/pic.h"
#include "sys/ps2.h"
#include "sys/ports.h"
#include "mmap.h"
#include "malloc.h"
#include "sys/io.h"
#include "io.h"
#include "sys/lba.h"
#include "mbr.h"
#include "fat.h"
#include "string.h"
#include "sys/vga.h"
#include "ui.h"
#include "elf.h"
#include "syscalls.h"

struct mbr *mbr = (void *)0x7c00;

#define MAX_CMD 1024


uint8_t mouse_packet[3];
uint8_t mouse_cycle = 0;

uint32_t mouse_x = 0;
uint32_t mouse_y = 0;

void handle_mouse() {
    outb(PIC2_COMMAND, 0x20);
    outb(PIC1_COMMAND, 0x20);

    uint8_t status = inb(PS2_COMMAND);
    if (status & 1) {
        uint8_t data = inb(PS2_DATA);
        mouse_packet[mouse_cycle++] = data;
        if (mouse_cycle == 3) {
            mouse_cycle = 0;
            uint32_t x = mouse_packet[1] - ((mouse_packet[0]>>4)&1)*256;
            uint32_t y = mouse_packet[2] - ((mouse_packet[0]>>5)&1)*256;

            mouse_x += x;
            mouse_y -= y;

            if ((int32_t)mouse_x < 0) mouse_x = 0;
            if ((int32_t)mouse_y < 0) mouse_y = 0;
            if (mouse_x >= 800) mouse_x = 799;
            if (mouse_y >= 600) mouse_y = 599;

            putpixel(mouse_x, mouse_y, WHITE);            
        }
    }

}

uint32_t tick_count = 0;

void handle_tick() {
    tick_count++;
    outb(PIC1_COMMAND, 0x20);
}

extern int test_syscall();

int main(void) {
    init_idt();
    kb_init();
    mouse_init();
    init_pit(1193182 / 1000);
    mask_interrupts();
    setup_fds();
    enable_interrupts();

    if(initSerial(coms[0])) {
        printf("ERROR : Failed to initialize serial port");
    }

    uint16_t low_memory = 1024;
    uint16_t upper_memory = *((uint16_t*)0x802);
    uint16_t extended_memory = *((uint16_t*)0x804);
    uint32_t available_memory = 1024*(low_memory + upper_memory + extended_memory * 64);
    printf("Memory available : %dKb\n", available_memory / 1024);
    
    if (init_paging(available_memory)) {
        setColor(RED);
        puts("Cannot init paging");
    }

    if (init_malloc()) {
        setColor(RED);
        puts("Cannot init malloc");
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

    load_elf("HELLOC.ELF");

    return 0;
    FILE *f = fopen("HELLO.TXT", "r");
    if (!f) {
        printf("Cannot open file\n");
        return 0;
    }

    char buf[32];
    uint32_t i = fread(buf, 1, 32, f);
    printf("%d '%s'\n", i, buf);
    memset(buf, 0, 32);
    fseek(f, 0, SEEK_SET);
    
    char *nod = "Noderyos";
    fwrite(nod, 1, strlen(nod), f);
    fseek(f, 0, SEEK_SET);
    i = fread(buf, 1, 32, f);
    printf("%d '%s'\n", i, buf);

    ui_clear(ui_rgb(254, 0, 0));
    ui_rect(40, 60, 70, 30, ui_rgba(0, 255, 0, 128));
    ui_circle(400, 300, 20, ui_rgb(0, 0, 255));
    ui_line(10, 10, 50, 30, ui_rgb(255, 255, 0));
    ui_triangle(246, 98, 72, 400, 542, 57, ui_rgb(255, 0, 255));

}
