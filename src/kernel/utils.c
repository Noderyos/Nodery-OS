#include "kernel/utils.h"

char* int2hex(char c){
	char *hex = {0,0,0}; // The target string
	char hc[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'}; // all hex chars
	hex[0] = hc[(int)c/16];
	hex[1] = hc[(int)c%16];
	hex[2] = 0; // So print can found the end of this string
	return hex;
}
