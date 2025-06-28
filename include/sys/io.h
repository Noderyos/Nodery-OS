#ifndef SYSIO_H
#define SYSIO_H

#include "../types.h"

#define FONT_WIDTH 1
#define FONT_HEIGHT 16
typedef enum {
    BLACK = 0x0,
    DARK_BLUE = 0x00007F,
    DARK_GREEN = 0x007F00,
    DARK_CYAN = 0x007F7F,
    DARK_RED = 0x7F0000,
    DARK_PINK = 0x7F007F,
    ORANGE = 0xFF7F00,
    LIGHT_GRAY = 0xB0B0B0,
    GREY = 0x7F7F7F,
    BLUE = 0x0000FF,
    GREEN = 0x00FF00,
    CYAN = 0x00FFFF,
    RED = 0xFF0000,
    PINK = 0xFF00FF,
    YELLOW = 0xFFFF00,
    WHITE = 0xFFFFFF
} TermColor;

extern u16 *coms;
extern u16 *lpts;

int initSerial(u16 port);
void writeSerial(u16 port, u8 c);
u8 readSerial(u16 port);

void setColor(TermColor color);
int putchar(int c);
void backspace();

void putpixel(u16 x, u16 y, u32 color);
#endif
