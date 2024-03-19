#ifndef IO_H
#define IO_H


#include "kernel/utils.h"
#include "asm/ports.h"

void print(u32 color, char* format, ...);

void swipeTerm();

void printNewLine();
void printChar(u32 color, char c);
#endif