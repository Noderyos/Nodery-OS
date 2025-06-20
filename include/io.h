#ifndef IO_H
#define IO_H

#include "sys/io.h"

void serialString(u16 port, char *str);

int puts(const char *s);
int printf(const char *format, ...);

#endif
