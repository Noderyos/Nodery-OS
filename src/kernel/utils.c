#include "kernel/utils.h"
#include "kernel/io.h" 

void int2hex(char c){
	char *hc = "0123456789ABCDEF";
	printChar(hc[c/16]);
  printChar(hc[c%16]);
}
