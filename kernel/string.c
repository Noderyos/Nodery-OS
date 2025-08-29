#include "string.h"

double strtod(char *nptr, char **endptr) {
    uint8_t is_neg = 0;
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


void *memset(void *s, uint8_t c, uint32_t n) {
    uint8_t *b = (uint8_t*)s;
    
    for (uint32_t i = 0; i < n; i++) {
        b[i] = c;
    }

    return s;
}

void *memcpy(void *dest, void *src, uint32_t n) {
    mempcpy(dest, src, n);
    return dest;
}

void *mempcpy(void *dest, void *src, uint32_t n) {
    uint8_t *d = (uint8_t*)dest;
    uint8_t *s = (uint8_t*)src;

    for (uint32_t i = 0; i < n; i++) {
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

char *stpncpy(char *dst, char *src, uint32_t dsize) {
    uint32_t dlen = strnlen(src, dsize);
    return memset(mempcpy(dst, src, dlen), 0, dsize - dlen);
}

char *strncpy(char *dst, char *src, uint32_t dsize) {
    stpncpy(dst, src, dsize);
    return dst;
}

char *strncat(char *dst, char *src, uint32_t n) {
    return strncpy(dst + strlen(dst), src, n);
}

int strncmp(char *s1, char *s2, uint32_t n) {
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

uint32_t strlen(char *str) {
    char *s2 = str;
    while (*s2) s2++;
    return s2-str;
}

uint32_t strnlen(char *s, uint32_t maxlen) {
    uint32_t len = strlen(s);
    return len < maxlen ? len : maxlen;
}


void itoa(int32_t value, char *buf, uint32_t base) {
    int idx = 0;
    uint8_t neg = 0;
    if (value < 0) {
        value = -value;
        buf[idx++] = '-';
        neg = 1;
    }
    while (value > 0) {
        uint8_t v = value % base;
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
            uint8_t tmp = buf[j];
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
    uint8_t ibuf[32];
    uint8_t j = 0;
    uint8_t is_f = 0;
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

