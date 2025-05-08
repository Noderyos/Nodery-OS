#include "sys/io.h"
#include "sys/ports.h"
#include "types.h"

#define VIDEO_BUFFER 0xB8000
#define SCR_WIDTH 80
#define SCR_HEIGHT 25

#define SCR_AT(x, y, i) (*(char *)((char*)VIDEO_BUFFER + ((y)*SCR_WIDTH+(x))*2+(i)))

int scr_x = 0;
int scr_y = 0;
TermColor color = WHITE;

void swipeTerm() {
    for (int y = 1; y < SCR_HEIGHT; y++) {
        for (int x = 0; x < SCR_WIDTH; x++) {
            SCR_AT(x, y-1, 0) = SCR_AT(x, y, 0);
            SCR_AT(x, y-1, 1) = SCR_AT(x, y, 1);
        }
    }
    for (int x = 0; x < SCR_WIDTH; x++) {
        SCR_AT(x, SCR_HEIGHT-1, 0) = ' ';
    }
}

void update_cursor(int x, int y){
	u16 pos = y * SCR_WIDTH + x;

	outb(0x3D4, 0x0F);
	outb(0x3D5, (u8) (pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (u8) ((pos >> 8) & 0xFF));
}

void newLine() {
    scr_y++;
    scr_x = 0;
    if (scr_y == SCR_HEIGHT) {
        swipeTerm();
        scr_y--;
    }
    update_cursor(scr_x, scr_y);
}

void setColor(TermColor c) {
    color = c;
}

int putchar(int c) {
    if (c == '\n') {
        newLine();
        return (u8)c;
    }

    SCR_AT(scr_x, scr_y, 0) = c;
    SCR_AT(scr_x, scr_y, 1) = color;

    scr_x++;
    if (scr_x == SCR_WIDTH) {
        newLine();
    }
    update_cursor(scr_x, scr_y);
    return (u8)c;
}

void backspace() {
    if(!scr_y) return;
    if(scr_x == 0) {
        scr_x = SCR_WIDTH;
        scr_y--;
    }
    scr_x--;
    SCR_AT(scr_x, scr_y, 0) = ' ';
    SCR_AT(scr_x, scr_y, 1) = PINK;
    update_cursor(scr_x, scr_y);
}
