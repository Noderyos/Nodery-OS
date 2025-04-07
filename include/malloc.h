#ifndef MALLOC_H
#define MALLOC_H

typedef struct {
    void *start;
    void *end;
}malloc_entry;

int init_malloc(unsigned int mem_size);
void *malloc(unsigned int size);
void *realloc(void *ptr, unsigned int new_size);
int free(void *ptr);

#endif
