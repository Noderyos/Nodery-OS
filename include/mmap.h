#ifndef MMAP_H
#define MMAP_H

#include "types.h"

// Heap parameters
#define PAGE_SIZE 4096
#define ALLOC_START 0x400000 // 4M

// Paging flags
#define PAGE_PRESENT 0b001
#define PAGE_RW      0b010
#define PAGE_USER    0b001

int init_paging(uint32_t available_memory);
void *mmap(uint32_t virt_addr, uint32_t len, int flags);
void *alloc_page();
int map_page(uint32_t phys_addr, uint32_t virt_addr, uint32_t flags);
 
#endif
