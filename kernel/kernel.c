#include "io.h"
#include "pic.h"
#include "ps2.h"
#include "ports.h"


#define KB_DATA 0x60
#define KB_COMMAND 0x64

void handle_keyboard() {
    u8 status = inb(KB_COMMAND);
    if (status & 1) {
        u8 keycode = inb(KB_DATA);
        printChar('C');
    }
    outb(PIC1_COMMAND, 0x20);
}

int main(void) {
    init_idt();
    kb_init();
    enable_interrupts();

    printString("Welcome to\n");
    setColor(YELLOW);
    printString("NoderyOS");
}
