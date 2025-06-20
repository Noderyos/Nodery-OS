#ifndef SYSIO_H
#define SYSIO_H

#include "../types.h"

typedef enum {
    BLACK = 0x0,
    DARK_BLUE,
    DARK_GREEN,
    DARK_CYAN,
    DARK_RED,
    DARK_PINK,
    ORANGE,
    LIGHT_GRAY,
    GREY,
    BLUE,
    GREEN,
    CYAN,
    RED,
    PINK,
    YELLOW,
    WHITE
} TermColor;

extern u16 *coms;
extern u16 *lpts;

int initSerial(u16 port);
void writeSerial(u16 port, u8 c);
u8 readSerial(u16 port);

void setColor(TermColor color);
int putchar(int c);
void backspace();
#endif
