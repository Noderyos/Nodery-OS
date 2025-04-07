#ifndef PS2_H
#define PS2_H

#include "types.h"

extern void keyboard_handler();
extern void int_handler();

void kb_init();
void handle_keyboard();

#endif
