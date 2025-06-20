# ifndef STRING_H
#define STRING_H

#include "types.h"

void *memcpy(void *dest, void *src, u32 n);
void *mempcpy(void *dest, void *src, u32 n);
void *memset(void *s, u8 c, u32 n);

char *stpcpy(char *dst, char *src);
char *strcpy(char *dst, char *src);
char *strncpy(char *dst, char *src, u32 dsize);
char *stpncpy(char *dst, char *src, u32 dsize);

char *strcat(char *dst, char *src);
char *strncat(char *dst, char *src, u32 n);

char *strchr(char *str, char c);
u32 strlen(char *str);

int strcmp(char *s1, char *s2);
int strncmp(char *s1, char *s2, u32 n);

u32 strnlen(char *s, u32 maxlen);

#endif
