#include "malloc.h"
#include "io.h"
#include "types.h"

#define START_ADDR 0x1000000
#define MALLOC_ENTRY_COUNT 256

uint32_t blob_size;
malloc_entry *malloc_entries;
void *malloc_blob;

int init_malloc(uint32_t mem_size){
    if(mem_size < START_ADDR + sizeof(malloc_entry) * MALLOC_ENTRY_COUNT)
        return -1;
    malloc_entries = (malloc_entry *)START_ADDR;
    malloc_blob = (void*)malloc_entries + sizeof(malloc_entry) * MALLOC_ENTRY_COUNT;
    blob_size = (void*)mem_size - malloc_blob;
    printf("Malloc entries at 0x%x array at 0x%x of size %dKb\n", malloc_entries, malloc_blob, blob_size/1024);

    return 0;
}

int find_empty_entry(){
    for (int i = 0; i < MALLOC_ENTRY_COUNT; ++i) {
        if(malloc_entries[i].start == 0)
            return i;
    }
    return -1;
}


int find_entry(void *ptr){
    for (int i = 0; i < MALLOC_ENTRY_COUNT; ++i) {
        if(malloc_entries[i].start == ptr)
            return i;
    }
    return -1;
}

void *realloc(void *ptr, uint32_t new_size){
    int entry = find_entry(ptr);
    if(entry == -1)
        return 0;

    void *new_ptr = malloc(new_size);
    if(!new_ptr)
        return 0;

    uint32_t old_size = malloc_entries[entry].size;
    uint32_t size = old_size < new_size ? old_size : new_size;

    for (uint32_t i = 0; i < size; ++i)
        *((uint8_t*)new_ptr + i) = *((uint8_t*)ptr + i);

    free(ptr);
    return new_ptr;
}


int find_up_nearest(void *ptr){
    void *min_addr = malloc_blob + blob_size;
    int min = -1;
    for (int i = 0; i < MALLOC_ENTRY_COUNT; ++i) {
        if(malloc_entries[i].start >= ptr && malloc_entries[i].start < min_addr){
            min_addr = malloc_entries[i].start;
            min = i;
        }
    }
    return min;
}

void *malloc(uint32_t size){
    int entry = find_empty_entry();
    if(entry < 0) return 0;

    void *cursor = malloc_blob;

    int near;
    while ((near = find_up_nearest(cursor)) >= 0                 // Mem allocated after
            && (uint32_t)(malloc_entries[near].start - cursor) < size // and not enough space
    ) cursor = malloc_entries[near].start + malloc_entries[near].size;

    // No memory allocated after and not enough space
    if (near < 0 && cursor + size > malloc_blob + blob_size) return 0;


    malloc_entries[entry].start = cursor;
    malloc_entries[entry].size = size;
    return cursor;
}

int free(void *ptr) {
    for (int i = 0; i < MALLOC_ENTRY_COUNT; ++i) {
        if(malloc_entries[i].start == ptr){
            malloc_entries[i].start = 0;
            malloc_entries[i].size = 0;
            return 0;
        }
    }
    return -1;
}
