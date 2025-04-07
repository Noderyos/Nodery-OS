#include "malloc.h"
#include "io.h"
#include "types.h"

#define START_ADDR 0x1000000
#define MALLOC_ENTRY_COUNT 256

u32 blob_size;
malloc_entry *malloc_entries;
void *malloc_blob;

int init_malloc(u32 mem_size){
    if(mem_size < START_ADDR + sizeof(malloc_entry) * MALLOC_ENTRY_COUNT)
        return -1;
    malloc_entries = (malloc_entry *)START_ADDR;
    malloc_blob = malloc_entries + sizeof(malloc_entry) * MALLOC_ENTRY_COUNT;
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

void *realloc(void *ptr, u32 new_size){
    int entry = find_entry(ptr);
    if(entry == -1)
        return 0;

    void *new_ptr = malloc(new_size);
    if(!new_ptr)
        return 0;

    u32 old_size = malloc_entries[entry].end - malloc_entries[entry].start + 1;
    u32 size = old_size < new_size ? old_size : new_size;

    for (u32 i = 0; i < size; ++i)
        *((unsigned char*)new_ptr + i) = *((unsigned char*)ptr + i);

    free(ptr);
    return new_ptr;
}


int find_up_nearest(void *ptr){
    void *min_addr = &malloc_blob[blob_size];
    int min = -1;
    for (int i = 0; i < MALLOC_ENTRY_COUNT; ++i) {
        if(malloc_entries[i].start >= ptr && malloc_entries[i].start < min_addr){
            min_addr = malloc_entries[i].start;
            min = i;
        }
    }
    return min;
}

void *malloc(u32 size){
    int entry = find_empty_entry();
    if(entry < 0) return 0;

    void *cursor = malloc_blob;

    int near;
next:
    near = find_up_nearest(cursor);
    if(near < 0){
        if(cursor+size <= (void*)&malloc_blob[blob_size]) goto malloc;
        else return 0;
    }else{
        u32 diff = malloc_entries[near].start - cursor;
        if(diff < size){
            cursor = malloc_entries[near].end + 1;
            goto next;
        }else{
            goto malloc;
        }
    }

malloc:
    malloc_entries[entry].start = cursor;
    malloc_entries[entry].end = cursor + size - 1;
    return cursor;
}

int free(void *ptr) {
    for (int i = 0; i < MALLOC_ENTRY_COUNT; ++i) {
        if(malloc_entries[i].start == ptr){
            malloc_entries[i].start = 0;
            malloc_entries[i].end = 0;
            return 0;
        }
    }
    return -1;
}
