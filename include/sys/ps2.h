#ifndef PS2_H
#define PS2_H

#include "types.h"


#define PS2_DATA 0x60
#define PS2_COMMAND 0x64

extern void keyboard_handler();
extern void mouse_handler();
extern void tick_handler();

void kb_init();
void mouse_init();
void handle_keyboard();
void handle_tick();
void handle_mouse();

void init_pit(uint16_t divisor);

#endif
