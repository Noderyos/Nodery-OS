#include "ui.h"
#include "sys/vga.h"
#include "sys/io.h"
#include "io.h"

#define oob(x, y) ((x) < 0 || (y) < 0 || (x) > 800 || (y) > 600)
#define abs(x) ((x)<0 ? -(x) : (x))

void ui_clear(uint32_t color) {
    struct vbe_mode_info_structure *vbe = (void *)0x7E00;
    
    void *fbuf = vbe->framebuffer;
    uint32_t c = 0;
    while (c++ < 800*600) {
        *(uint32_t*)fbuf = color;
        fbuf += 3;
    }
}

void ui_rect(int16_t x, int16_t y, uint16_t w, uint16_t h, uint32_t color) {
    for (int16_t cy = y; cy < y+h; cy++)
        for (int16_t cx = x; cx < x + w; cx++)
            if (!oob(cx, cy))
                putpixel((uint16_t)cx, (uint16_t)cy, color);
}

void ui_circle(int16_t x, int16_t y, uint16_t r, uint32_t color) {
    for (int16_t dy = -r; dy < r; dy++) {
        for (int16_t dx = -r; dx < r; dx++) {
            int16_t cx = x + dx;
            int16_t cy = y + dy;
            if (!oob(cx, cy))
                if(dx*dx + dy*dy < r*r)
                    putpixel(cx, cy, color);
        }
    }
}

void ui_line(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint32_t color) {
    int16_t dx = x2 - x1;
    int16_t dy = y2 - y1;

    if (dx == 0 && dy == 0) {
        putpixel(x1, y1, color);
        return;
    }

    if (abs(dx) > abs(dy)) {
        if (x1 > x2) {int16_t tmp=x1;x1=x2;x2=tmp;}
        
        for (int x = x1; x <= x2; x++) {
            int16_t y = dy*(x - x1)/dx + y1;
            if (!oob(x, y)) putpixel(x, y, color);    
        }

    } else {
        if (y1 > y2) {int16_t tmp=y1;y1=y2;y2=tmp;}
    
        for (int y = y1; y <= y2; y++) {
            int16_t x = dx*(y - y1)/dy + x1;
            if (!oob(x, y)) putpixel(x, y, color);    
        }
    }
}


int16_t interpolate_x(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t y) {
    return x1 + (x2-x1)*(y-y1)/(y2-y1);
}

void ui_triangle(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3, uint32_t color) {
    if (y1 > y2) {int16_t tx=x1,ty=y1;x1=x2;y1=y2;x2=tx;y2=ty;}
    if (y2 > y3) {int16_t tx=x2,ty=y2;x2=x3;y2=y3;x3=tx;y3=ty;}
    if (y1 > y2) {int16_t tx=x1,ty=y1;x1=x2;y1=y2;x2=tx;y2=ty;}


    for (int16_t y = y1; y <= y3; y++) {
        int16_t xa, xb;
        if (y < y2) {
            xa = interpolate_x(x1, y1, x2, y2, y);
            xb = interpolate_x(x1, y1, x3, y3, y);
        } else {
            xa = interpolate_x(x2, y2, x3, y3, y);
            xb = interpolate_x(x1, y1, x3, y3, y);
        }
        ui_line(xa, y, xb, y, color);
    }
}
