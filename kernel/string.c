#include "string.h"

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
