#ifndef UI_H
#define UI_H

#include "types.h"

#define ui_rgba(r, g, b, a) ((a) << 24 | (r)<<16 | (g) << 8 | (b))
#define ui_rgb(r, g, b) ui_rgba(r, g, b, 255)

void ui_rect(int16_t x, int16_t y, uint16_t w, uint16_t h, uint32_t color);
void ui_circle(int16_t x, int16_t y, uint16_t r, uint32_t color);
void ui_line(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint32_t color);
void ui_triangle(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3, uint32_t color);
void ui_clear(uint32_t color);


#endif
