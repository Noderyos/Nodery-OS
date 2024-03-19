#include "kernel/io.h"
#include "vga_fonts.h"
#include "vga.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define FONT_COLS 5
#define FONT_ROWS 7
#define FONT_SIZE 3

int x = 10;
int y = 10;

vbe_mode_info_structure *vbe = (void *)0x7E00;


void print(u32 color, char* format, ...){
  register int i asm("ebp");
  unsigned int *args = (unsigned int *)(i + 12);
  char itoa_buf[64];
  int form = 0;
  int index = 1;
  for(char c = *format++; c != 0; c = *format++){
    if(form){
      form = 0;
      if(c == 'd'){
        _itoa(args[index++], itoa_buf, 10);
        int i = 0;
        while (itoa_buf[i])
          printChar(color, itoa_buf[i++]);
        
      }else if(c == 'x'){
        _itoa(args[index++], itoa_buf, 16);
        int i = 0;
        while (itoa_buf[i])
          printChar(color, itoa_buf[i++]);
      }else if(c == 's'){
        char *str = (char *)args[index++];
        int j = 0;
        while(str[j])
          printChar(color, str[j++]);
      }
    }else{
      if(c == '%')
        form = 1;
      else
        printChar(color, c);
    }
  }
}

void printChar(u32 color, char c){
  if(c == '\n'){ // Detect new line char
    printNewLine();
    return;
  }


    u8 *letter = font7x5[c];

    for (int i = 0; i < 5; ++i){

        for (int __ = 0; __ < FONT_SIZE; ++__)
        {
            u8 part = letter[i];
            for (int j = 0; j < 7; ++j){
                if(part & 1){
                    for (int _ = 0; _ < FONT_SIZE; ++_)
                    {
                        putpixel(vbe->framebuffer, x+i*FONT_SIZE + __, y+j*FONT_SIZE + _, color);
                    }
                }
                part >>= 1;
            }
        }
    }
  x += (FONT_COLS + 1) * FONT_SIZE;
  if(x >= SCREEN_WIDTH-(FONT_COLS + 1) * FONT_SIZE){
    printNewLine();
  }
}

void printNewLine(){
    x = 10;
    y += (FONT_ROWS + 2) * FONT_SIZE;
  if(y > SCREEN_HEIGHT - (FONT_ROWS + 2) * FONT_SIZE){
    swipeTerm();
      y -= (FONT_ROWS + 2) * FONT_SIZE;
  }
}

void swipeTerm(){
    u8 *out = (u8*)vbe->framebuffer;
    u8 *in = &((u8*)vbe->framebuffer)[(FONT_ROWS*2 - 2)*FONT_SIZE*SCREEN_HEIGHT*3];
    for (int i = 0; i < (SCREEN_WIDTH-(FONT_ROWS*2 - 2)*FONT_SIZE)*SCREEN_HEIGHT*3; ++i) {
        out[i] = in[i];
    }

    u8 *clear = &((u8*)vbe->framebuffer)[(SCREEN_WIDTH-(FONT_ROWS*2 - 2)*FONT_SIZE)*SCREEN_HEIGHT*3];
    for (int i = 0; i < (FONT_ROWS*2 - 2)*FONT_SIZE * SCREEN_HEIGHT * 3; ++i) {
        clear[i] = 0;
    }
}