#ifndef SYSIO_H
#define SYSIO_H

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

void setColor(TermColor color);
int putchar(int c);
void backspace();
#endif
