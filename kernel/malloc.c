#include "malloc.h"
#include "io.h"
#include "types.h"
#include "mmap.h"

#define INITIAL_SIZE (16*PAGE_SIZE) // 64k
#define START_ADDR 0x50000000

typedef struct block {
    uint32_t size;
    int free;
    struct block *next;
} block_t;

block_t *malloc_start;
void *malloc_end;

int init_malloc(){
    malloc_start = (block_t *)START_ADDR;
    malloc_end = malloc_start + INITIAL_SIZE;
    if (!mmap((uint32_t)malloc_start, INITIAL_SIZE, PAGE_PRESENT | PAGE_RW)) return -1;
    malloc_start->free = 1;
    malloc_start->size = INITIAL_SIZE;
    malloc_start->next = 0;
    return 0;
}

void *malloc_expand(uint32_t size) {
    uint32_t alligned_size = (size + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);

    for (uint32_t i = 0; i < alligned_size; i += PAGE_SIZE) {
        void *virt = malloc_end + i;
        void *phys = alloc_page();
        if (!phys) return 0;
        if (!map_page((uint32_t)virt, (uint32_t)phys, PAGE_PRESENT | PAGE_RW)) return 0;
    }
    void *old_end = malloc_end;
    malloc_end += alligned_size;
    return old_end;
}

void *realloc(void *ptr, uint32_t new_size){
    void *new_ptr = malloc(new_size);
    if(!new_ptr) return 0;


    uint32_t old_size = ((block_t *)(ptr-sizeof(block_t)))->size;
    uint32_t size = old_size < new_size ? old_size : new_size;

    for (uint32_t i = 0; i < size; ++i)
        *((uint8_t*)new_ptr + i) = *((uint8_t*)ptr + i);

    free(ptr);
    return new_ptr;
}

void *malloc(uint32_t size){
    block_t *current = malloc_start;
    while(current) {
        if(current->free && current->size >= size) {
            if (current->size >= size + sizeof(block_t) + 4) { // 4 is a margin
                block_t *new_block = (block_t *)((uint8_t*)current + sizeof(block_t) + size);
                new_block->size = current->size - size - sizeof(block_t);
                new_block->free = 1;
                new_block->next = current->next;
                current->next = new_block;
                current->size = size;
            }
            current->free = 0;
            return (void*)current + sizeof(block_t);
        }
        current = current->next;
    }
    uint32_t total_size = size + sizeof(block_t);
    current = malloc_expand(total_size);
    if (!current) return 0;

    current->free = 0;
    current->size = size;

    uint32_t leftover = ((uint32_t)malloc_end - (uint32_t)current) - size;
    if (leftover > sizeof(block_t) + 4) {
        block_t *next = current + total_size;
        next->size = leftover - sizeof(block_t);
        next->free = 1;
        next->next = 0;
        current->next = next;
    } else {
        current->next = 0;
    }
    return (uint8_t*)current + sizeof(block_t);
}

int free(void *ptr) {
    if (!ptr) return 0;
    block_t *block = ptr - sizeof(block_t);
    block->free = 1;

    block_t *current = malloc_start;
    while(current && current->next) {
        if(current->free && current->next->free) {
            current->size += sizeof(block_t) + current->next->size;
            current->next = current->next->next;
        } else {
            current = current->next;
        }
    }
    return 0;
}
