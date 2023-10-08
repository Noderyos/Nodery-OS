#include "kernel/io.h"

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25
#define VIDEO_ADDRESS 0xb8000
// Video address usage :
// to print a char you need to specify the char and the color code 
// [First char ascii code][First char color code]
// [Second char ascii code][Second char color code]
// [Third char ascii code][Third char color code]

extern void ioport_out(unsigned short port, unsigned char data);

int curW;
int curH;

void print(char* format, ...){
  register int i asm("ebp");
  unsigned int *args = (unsigned int *)(i + 8);
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
          printChar(itoa_buf[i++]);
        
      }else if(c == 'x'){
        _itoa(args[index++], itoa_buf, 16);
        int i = 0;
        while (itoa_buf[i])
          printChar(itoa_buf[i++]);
      }else if(c == 's'){
        char *str = (char *)args[index++];
        int j = 0;
        while(str[j])
          printChar(str[j++]);
      }
    }else{
      if(c == '%')
        form = 1;
      else
        printChar(c);
    }
  }
}


void updateCursor(){
  unsigned short pos = (curH * SCREEN_WIDTH + curW);
  ioport_out(0x3D4, 0x0F);
  ioport_out(0x3D5, (unsigned char) (pos & 0xFF));
  ioport_out(0x3D4, 0x0E);
  ioport_out(0x3D5, (unsigned char) ((pos >> 8) & 0xFF));
}

void printChar(char c){
  if(c == '\n'){ // Detect new line char
    printNewLine();
    return;
  }
  *(char*)(VIDEO_ADDRESS + (curH*SCREEN_WIDTH*2) + (curW*2)) = c;
  curW++;
  if(curW >= SCREEN_WIDTH){
    printNewLine();
  }
  updateCursor();
  return;
}

void printString(char* string){
  for (int i = 0; 1; i++) {
    char character = (int) string[i];
    if (character == '\0') {
      return;
    }
    printChar(character);
  }
  return;
}

void printNewLine(){
  curW = 0;
  curH++;
  if(curH == SCREEN_HEIGHT){
    swipeTerm();
    curH--;
  }
  updateCursor();
  return;
}

void swipeTerm(){
  for(int i = 1; i < SCREEN_HEIGHT;i++){
    for(int j = 0; j < SCREEN_WIDTH;j++){
      *(char*)(VIDEO_ADDRESS + (i*SCREEN_HEIGHT*2) + (j*2) - SCREEN_WIDTH*2) = *(char*)(VIDEO_ADDRESS + (i*SCREEN_HEIGHT*2) + (j*2));
    } 
  }
  for(int i = 0; i < SCREEN_WIDTH;i++){
    *(char*)(VIDEO_ADDRESS + (SCREEN_WIDTH*(SCREEN_HEIGHT-1)*2)+i*2) = ' ';
  }
}

void moveCursor(int w,int h){
  curW = w;
  curH = h;
}
