#ifndef MMAP_H
#define MMAP_H

#include "types.h"

// Kernel page directory
#define PD_ADDR ((uint32_t*)0xFFFFF000)
#define PT_ADDR ((uint32_t*)0xFFC00000)

// Heap parameters
#define PAGE_SIZE 4096
#define ALLOC_START 0x400000 // 4M

// Paging flags
#define PAGE_PRESENT 0b001
#define PAGE_RW      0b010
#define PAGE_USER    0b100
#define PAGE_DEFAULT (PAGE_PRESENT|PAGE_RW)

int init_paging(uint32_t available_memory);
void *alloc_page();
void *mmap(uint32_t *pd, void *virt_addr, uint32_t len, int flags);
int map_page(uint32_t *pd, void *phys_addr, void *virt_addr, uint32_t flags);

#endif
