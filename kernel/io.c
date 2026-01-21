#include "sys/io.h"
#include "sys/ports.h"
#include "io.h"
#include "types.h"
#include "string.h"

#define va_start(ap, last) ((ap) = ((void*)&(last))+sizeof(last))
#define va_arg(ap, type) (*({type* tmp = ap;(ap)+=sizeof(void*);tmp;}))


void serial_string(uint16_t port, char *str) {
    while(*str)
        write_serial(port, *str++);
}

void serial_int(uint16_t port, uint32_t a) {
    if (a > 9) serial_int(port, a/10);
    write_serial(port, '0' + (a%10));
}

int printf(const char *format, ...) {
    void* args;
    va_start(args, format);
    
    char buf[32];
    uint8_t j = 0;
    uint8_t is_f = 0;
    char *str;
    while (*format) {
        if (is_f) {
            is_f = 0;
            if (*format == '%') {
                putchar('%');
            } else {
                switch (*format) {
                    case 'c':
                        putchar(va_arg(args, char));
                        break;
                    case 'd':
                        itoa(va_arg(args, int), buf, 10);
                        j = 0;
                        while(buf[j]) {
                            putchar(buf[j++]);
                        }
                        break;
                    case 'x':
                        itoa(va_arg(args, int), buf, 16);
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

int puts(const char *s) {
    while (*s != '\0') putchar(*s++);
    putchar('\n');
    return 0;
}
