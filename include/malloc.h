#ifndef MALLOC_H
#define MALLOC_H

#include "types.h"

int init_malloc();
void *malloc(unsigned int size);
void *realloc(void *ptr, unsigned int new_size);
int free(void *ptr);

#endif
