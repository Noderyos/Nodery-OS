#ifndef UI_H
#define UI_H

#include "types.h"

#define ui_rgba(r, g, b, a) ((a) << 24 | (r)<<16 | (g) << 8 | (b))
#define ui_rgb(r, g, b) ui_rgba(r, g, b, 255)

void ui_rect(i16 x, i16 y, u16 w, u16 h, u32 color);
void ui_circle(i16 x, i16 y, u16 r, u32 color);
void ui_line(i16 x1, i16 y1, i16 x2, i16 y2, u32 color);
void ui_triangle(i16 x1, i16 y1, i16 x2, i16 y2, i16 x3, i16 y3, u32 color);
void ui_clear(u32 color);


#endif
