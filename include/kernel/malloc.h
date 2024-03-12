#ifndef MALLOC_H
#define MALLOC_H

#define MALLOC_ENTRY_COUNT 512

#include "kernel/io.h"

typedef struct {
    void *start;
    void *end;
}malloc_entry;

int init_malloc(unsigned int mem_size);
void *malloc(unsigned int size);
int free(void *ptr);

#endif
