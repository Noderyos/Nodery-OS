#include "kernel/utils.h"


void _itoa(int integer, char* result, int base) {
	int num_digits = 0;
	int val = 0;
	while (integer > 0) {
		val = integer % base;
		if (val < 10) {
			result[num_digits] = val + '0';
		} else {
			result[num_digits] = val - 10 + 'A';
		}
		num_digits++;
		integer /= base;
	}

  char tmp = 0;
  for (int i = 0; i < num_digits/2; i++) {
      tmp = result[i];
      result[i] = result[num_digits-i-1];
      result[num_digits-i-1] = tmp;
  }
	if (num_digits == 0) {
		num_digits = 1;
		result[0] = '0';
	}
  result[num_digits] = '\0';
}

