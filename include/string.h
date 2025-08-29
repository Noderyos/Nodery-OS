# ifndef STRING_H
#define STRING_H

#include "types.h"

void *memcpy(void *dest, void *src, uint32_t n);
void *mempcpy(void *dest, void *src, uint32_t n);
void *memset(void *s, uint8_t c, uint32_t n);

char *stpcpy(char *dst, char *src);
char *strcpy(char *dst, char *src);
char *strncpy(char *dst, char *src, uint32_t dsize);
char *stpncpy(char *dst, char *src, uint32_t dsize);

char *strcat(char *dst, char *src);
char *strncat(char *dst, char *src, uint32_t n);

char *strchr(char *str, char c);
uint32_t strlen(char *str);

int strcmp(char *s1, char *s2);
int strncmp(char *s1, char *s2, uint32_t n);

uint32_t strnlen(char *s, uint32_t maxlen);

void itoa(int32_t value, char *buf, uint32_t base);

double strtod(char *nptr, char **endptr);
int sprintf(char *buf, const char *format, ...);
#endif
