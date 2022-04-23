#include "print.h"

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25
#define VIDEO_ADDRESS 0xb8000

int curW;
int curH;

void printChar(char c){
	if(c == '\n'){
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