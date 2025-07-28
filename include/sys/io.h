#ifndef SYSIO_H
#define SYSIO_H

#include "../types.h"

#define FONT_WIDTH 1
#define FONT_HEIGHT 16
typedef enum {
    BLACK = 0xFF0,
    DARK_BLUE = 0xFF00007F,
    DARK_GREEN = 0xFF007F00,
    DARK_CYAN = 0xFF007F7F,
    DARK_RED = 0xFF7F0000,
    DARK_PINK = 0xFF7F007F,
    ORANGE = 0xFFFF7F00,
    LIGHT_GRAY = 0xFFB0B0B0,
    GREY = 0xFF7F7F7F,
    BLUE = 0xFF0000FF,
    GREEN = 0xFF00FF00,
    CYAN = 0xFF00FFFF,
    RED = 0xFFFF0000,
    PINK = 0xFFFF00FF,
    YELLOW = 0xFFFFFF00,
    WHITE = 0xFFFFFFFF
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
