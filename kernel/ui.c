#include "ui.h"
#include "sys/vga.h"
#include "sys/io.h"
#include "io.h"

#define oob(x, y) ((x) < 0 || (y) < 0 || (x) > 800 || (y) > 600)
#define abs(x) ((x)<0 ? -(x) : (x))

void ui_clear(u32 color) {
    struct vbe_mode_info_structure *vbe = (void *)0x7E00;
    
    void *fbuf = vbe->framebuffer;
    u32 c = 0;
    while (c++ < 800*600) {
        *(u32*)fbuf = color;
        fbuf += 3;
    }
}

void ui_rect(i16 x, i16 y, u16 w, u16 h, u32 color) {
    for (i16 cy = y; cy < y+h; cy++)
        for (i16 cx = x; cx < x + w; cx++)
            if (!oob(cx, cy))
                putpixel((u16)cx, (u16)cy, color);
}

void ui_circle(i16 x, i16 y, u16 r, u32 color) {
    for (i16 dy = -r; dy < r; dy++) {
        for (i16 dx = -r; dx < r; dx++) {
            i16 cx = x + dx;
            i16 cy = y + dy;
            if (!oob(cx, cy))
                if(dx*dx + dy*dy < r*r)
                    putpixel(cx, cy, color);
        }
    }
}

void ui_line(i16 x1, i16 y1, i16 x2, i16 y2, u32 color) {
    i16 dx = x2 - x1;
    i16 dy = y2 - y1;

    if (dx == 0 && dy == 0) {
        putpixel(x1, y1, color);
        return;
    }

    if (abs(dx) > abs(dy)) {
        if (x1 > x2) {i16 tmp=x1;x1=x2;x2=tmp;}
        
        for (int x = x1; x <= x2; x++) {
            i16 y = dy*(x - x1)/dx + y1;
            if (!oob(x, y)) putpixel(x, y, color);    
        }

    } else {
        if (y1 > y2) {i16 tmp=y1;y1=y2;y2=tmp;}
    
        for (int y = y1; y <= y2; y++) {
            i16 x = dx*(y - y1)/dy + x1;
            if (!oob(x, y)) putpixel(x, y, color);    
        }
    }
}


i16 interpolate_x(i16 x1, i16 y1, i16 x2, i16 y2, i16 y) {
    return x1 + (x2-x1)*(y-y1)/(y2-y1);
}

void ui_triangle(i16 x1, i16 y1, i16 x2, i16 y2, i16 x3, i16 y3, u32 color) {
    if (y1 > y2) {i16 tx=x1,ty=y1;x1=x2;y1=y2;x2=tx;y2=ty;}
    if (y2 > y3) {i16 tx=x2,ty=y2;x2=x3;y2=y3;x3=tx;y3=ty;}
    if (y1 > y2) {i16 tx=x1,ty=y1;x1=x2;y1=y2;x2=tx;y2=ty;}


    for (i16 y = y1; y <= y3; y++) {
        i16 xa, xb;
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
