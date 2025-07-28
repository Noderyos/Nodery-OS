#include "string.h"

double strtod(char *nptr, char **endptr) {
    u8 is_neg = 0;
    if (*nptr == '-') { is_neg = 1; nptr++; }
    int int_part = 0;
    double float_part = 0.f;
    float div = 10.f;
    while (*nptr && *nptr != '.') {
        if (*nptr > '9' || *nptr < '0') break;
        int_part = int_part *10 + (*nptr-'0');
        nptr++;
    }
    if (*nptr == '.') {
        nptr++;
        while (*nptr) {
            if (*nptr > '9' || *nptr < '0') break;
            float_part = float_part + (*nptr-'0')/div;
            div *= 10;
            nptr++;
        }
    }

    return (is_neg ? -1 : 1) * (int_part + float_part);
}


void *memset(void *s, u8 c, u32 n) {
    u8 *b = (u8*)s;
    
    for (u32 i = 0; i < n; i++) {
        b[i] = c;
    }

    return s;
}

void *memcpy(void *dest, void *src, u32 n) {
    mempcpy(dest, src, n);
    return dest;
}

void *mempcpy(void *dest, void *src, u32 n) {
    u8 *d = (u8*)dest;
    u8 *s = (u8*)src;

    for (u32 i = 0; i < n; i++) {
        d[i] = s[i];
    }

    return d + n;
}


char *stpcpy(char *dst, char *src) {
    char *p = mempcpy(dst, src, strlen(src));
    *p = 0;
    return p;
}

char *strcpy(char *dst, char *src) {
    stpcpy(dst, src);
    return dst;
}

char *strcat(char *dst, char *src) {
    stpcpy(dst + strlen(dst), src);
    return dst;
}

int strcmp(char *s1, char *s2) {
    while (*s1 || *s2) {
        if (*s1 != *s2) return *s1 - *s2;
        s1++;
        s2++;
    }
    return 0;
}

char *stpncpy(char *dst, char *src, u32 dsize) {
    u32 dlen = strnlen(src, dsize);
    return memset(mempcpy(dst, src, dlen), 0, dsize - dlen);
}

char *strncpy(char *dst, char *src, u32 dsize) {
    stpncpy(dst, src, dsize);
    return dst;
}

char *strncat(char *dst, char *src, u32 n) {
    return strncpy(dst + strlen(dst), src, n);
}

int strncmp(char *s1, char *s2, u32 n) {
    while ((*s1 || *s2) && n) {
        if (*s1 != *s2) return *s1 - *s2;
        s1++;
        s2++;
        n--;
    }
    return 0;
}

char *strchr(char *str, char c) {
    while (*str && *str != c) str++;
    return *str == 0 ? 0 : str;
}

u32 strlen(char *str) {
    char *s2 = str;
    while (*s2) s2++;
    return s2-str;
}

u32 strnlen(char *s, u32 maxlen) {
    u32 len = strlen(s);
    return len < maxlen ? len : maxlen;
}


void itoa(i32 value, char *buf, u32 base) {
    int idx = 0;
    u8 neg = 0;
    if (value < 0) {
        value = -value;
        buf[idx++] = '-';
        neg = 1;
    }
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
    if (idx) {
        for(int j = neg; j < idx/2; j++) {
            u8 tmp = buf[j];
            buf[j] = buf[idx-j-1];
            buf[idx-j-1] = tmp;
        }
    } else {
        buf[idx++] = '0';
    }
    buf[idx] = '\0';
}


#define va_start(ap, last) ((ap) = ((void*)&(last))+sizeof(last))
#define va_arg(ap, type) (*({type* tmp = ap;(ap)+=sizeof(void*);tmp;}))

int sprintf(char *buf, const char *format, ...) {
    void* args;
    va_start(args, format);
    u8 ibuf[32];
    u8 j = 0;
    u8 is_f = 0;
    char *str;
    while (*format) {
        if (is_f) {
            is_f = 0;
            if (*format == '%') {
                *buf++ = '%';
            } else {
                switch (*format) {
                    case 'c':
                        *buf++ = va_arg(args, char);
                        break;
                    case 'd':
                        itoa(va_arg(args, int), ibuf, 10);
                        buf = stpcpy(buf, ibuf);
                        break;
                    case 'x':
                        itoa(va_arg(args, int), ibuf, 16);
                        buf = stpcpy(buf, ibuf);
                        break;
                    case 's':
                        str = va_arg(args, char *);
                        buf = stpcpy(buf, str);
                        break;
                }
            }
        } else {
            if (*format == '%') {
                is_f = 1;
            } else {
                *buf++ = *format;
            }
        }
        format++;
    }
    return 0;
}

