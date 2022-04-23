#include "print.h"
#include "keyboard.h"

extern void main(){
	init_idt();
	kb_init();
	enable_interrupts();
	printString("Welcome to NoderyOS\n");

	while(1){
		if(handle_keyboard_interrupt() != (char)166){
			printString("INT");
		}
	}
}
