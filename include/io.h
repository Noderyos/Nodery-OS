#ifndef IO_H
#define IO_H

#include "sys/io.h"

void serial_string(uint16_t port, char *str);
void serial_int(uint16_t port, uint32_t i);

int puts(const char *s);
int printf(const char *format, ...);

#endif
