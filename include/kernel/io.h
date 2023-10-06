#ifndef IO_H
#define IO_H

#include <stdint.h>
#include <stddef.h>

void swipeTerm();

void printNewLine();
void printChar(char c); // Allow you to print a char
void printString(char* string); // Allow you to print a string
void moveCursor(int w,int h);  // Allow you to move the print cursor (because the print cursor move automaticaly after a print)
#endif
