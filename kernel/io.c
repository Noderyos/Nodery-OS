#include "io.h"

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

void newLine() {
    scr_y++;
    scr_x = 0;
    if (scr_y == SCR_HEIGHT) {
        swipeTerm();
        scr_y--;
    }
}

void setColor(TermColor c) {
    color = c;
}


void printChar(char c) {
    if (c == '\n') {
        newLine();
        return;
    }

    SCR_AT(scr_x, scr_y, 0) = c;
    SCR_AT(scr_x, scr_y, 1) = color;

    scr_x++;
    if (scr_x == SCR_WIDTH) {
        newLine();
    }
}


void printString(char *s) {
    while (*s != '\0')
        printChar(*s++);
}
