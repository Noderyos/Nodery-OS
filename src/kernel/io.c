#include "io.h"

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25
#define VIDEO_ADDRESS 0xb8000
// Video address usage :
// to print a char you need to specify the char and the color code 
// [First char ascii code][First char color code]
// [Second char ascii code][Second char color code]
// [Third char ascii code][Third char color code]


int curW;
int curH;

void printChar(char c){
	if(c == '\n'){ // Detect new line char
		printnewLine();
		return;
	}
	*(char*)(VIDEO_ADDRESS + (curH*SCREEN_WIDTH*2) + (curW*2)) = c;
	curW++;
	if(curW >= SCREEN_WIDTH){
		printnewLine();
	}
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

void printnewLine(){
	curW = 0;
	curH++;
	if(curH == SCREEN_HEIGHT){
		swipeTerm();
		curH--;
	}
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
