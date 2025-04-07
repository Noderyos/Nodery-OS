#include "io.h"
#include "types.h"
#include "ports.h"

#define SCR_AT(x, y, i) (*(char *)((char*)VIDEO_BUFFER + ((y)*SCR_WIDTH+(x))*2+(i)))

#define va_start(ap, last) ((ap) = ((void*)&(last))+sizeof(last))
#define va_arg(ap, type) (*({type* tmp = ap;(ap)+=sizeof(void*);tmp;}))

int scr_x = 0;
int scr_y = 0;
TermColor color = WHITE;


void _itoa(u32 value, char *buf, u32 base) {
    int idx = 0;
    while (value > 0) {
        u8 v = value % base;
        if (v < 10) {
            buf[idx] = '0' + v;
        } else {
            buf[idx] = 'A' + v - 10;
        }
        idx++;
        value /= base;
    }

    for(int j = 0; j < idx/2; j++) {
        u8 tmp = buf[j];
        buf[j] = buf[idx-j-1];
        buf[idx-j-1] = tmp;
    }
    buf[idx] = '\0';
}

int printf(const char *format, ...) {
    void* args;
    va_start(args, format);
    
    char buf[32];
    u8 j = 0;
    u8 is_f = 0;
    char *str;
    while (*format) {
        if (is_f) {
            is_f = 0;
            if (*format == '%') {
                putchar('%');
            } else {
                switch (*format) {
                    case 'd':
                        _itoa(va_arg(args, int), buf, 10);
                        j = 0;
                        while(buf[j]) {
                            putchar(buf[j++]);
                        }
                        break;
                    case 'x':
                        _itoa(va_arg(args, int), buf, 16);
                        j = 0;
                        while(buf[j]) {
                            putchar(buf[j++]);
                        }
                        break;
                    case 's':
                        str = va_arg(args, char *);
                        while(*str) putchar(*str++);
                        break;
                    default:
                        buf[0] = 'E';
                        buf[1] = 'R';
                        buf[2] = 'R';
                }
            }
        } else {
            if (*format == '%') {
                is_f = 1;
            } else {
                putchar(*format);
            }
        }
        format++;
    }


    return 0;
}

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


int puts(const char *s) {
    while (*s != '\0')
        putchar(*s++);
    putchar('\n');
    return 0;
}
