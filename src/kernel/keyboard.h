#pragma once

#include <stdint.h>
#include <stddef.h>

void init_idt();
void kb_init();
char handle_keyboard_interrupt();