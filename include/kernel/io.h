#ifndef IO_H
#define IO_H


#include "kernel/utils.h"
#include "asm/ports.h"

typedef enum
{
	BLACK  = 0x0, BLUE   = 0x1, GREEN  = 0x2, CYAN   = 0x3,
	RED    = 0x4, PURLE  = 0x5, OANGE  = 0x6, LGREY  = 0x7,
	GREY   = 0x8, LBLUE  = 0x9, LGREEN = 0xA, LCYAN  = 0xB,
	LRED   = 0xC, PINK   = 0xD, YELLOW = 0xE, WHITE  = 0xF
} term_color;

void print(char* format, ...);

void updateCursor();

void swipeTerm();

void setForeground(term_color fg);
void setBackground(term_color bg);
void setColor(term_color bg, term_color fg);


void printNewLine();
void printChar(char c); // Print caracter
void moveCursor(int w,int h);  // Allow you to move the print cursor (because the print cursor move automaticaly after a print)
#endif
