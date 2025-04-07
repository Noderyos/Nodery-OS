#ifndef IO_H
#define IO_H

#define VIDEO_BUFFER 0xB8000
#define SCR_WIDTH 80
#define SCR_HEIGHT 25

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
int puts(const char *s);
void backspace();
#endif
