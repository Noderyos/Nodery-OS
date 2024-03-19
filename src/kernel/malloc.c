#include "kernel/malloc.h"
#include "kernel/io.h"

unsigned int BLOB_SIZE = 0;
malloc_entry *malloc_entries = 0;
unsigned char *malloc_blob = 0;

int init_malloc(unsigned int mem_size){
    unsigned int start_addr = 0x01000000;

    if(mem_size < start_addr + sizeof(malloc_entry) * MALLOC_ENTRY_COUNT)
        return -1;
    malloc_entries = (malloc_entry *)start_addr;
    malloc_blob = (u8*)(start_addr + sizeof(malloc_entry) * MALLOC_ENTRY_COUNT);
    BLOB_SIZE = mem_size - start_addr - sizeof(malloc_entry) * MALLOC_ENTRY_COUNT;
    print(0xFFFFFF, "Malloc entries at 0x%x array at 0x%x of size %dKb\n", malloc_entries, malloc_blob, BLOB_SIZE/1024);

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

void *realloc(void *ptr, unsigned int new_size){
    int entry_idx = find_entry(ptr);
    if(entry_idx == -1)
        return 0;

    unsigned int size = malloc_entries[entry_idx].end - malloc_entries[entry_idx].start + 1;
    if(size > new_size)
        return 0;

    void *new_ptr = malloc(new_size);
    if(!new_ptr)
        return 0;

    for (int i = 0; i < size; ++i)
        *((unsigned char*)new_ptr + i) = *((unsigned char*)ptr + i);

    free(ptr);
    return new_ptr;
}


int find_up_nearest(void *ptr){
    void *min_addr = &malloc_blob[BLOB_SIZE];
    int min = -1;
    for (int i = 0; i < MALLOC_ENTRY_COUNT; ++i) {
        if(malloc_entries[i].start >= ptr && malloc_entries[i].start < min_addr){
            min_addr = malloc_entries[i].start;
            min = i;
        }
    }
    return min;
}

void *malloc(unsigned int size){
    int entry = find_empty_entry();
    if(entry < 0) return 0;

    void *cursor = malloc_blob;

    int near;
next: // Until we have found the right place
    near = find_up_nearest(cursor);
    if(near < 0){
        if(cursor+size <= (void*)&malloc_blob[BLOB_SIZE]) goto malloc;
        else return 0;
    }else{
        long diff = malloc_entries[near].start - cursor;
        if(diff < size){
            cursor = malloc_entries[near].end + 1;
            goto next;
        }else{
            goto malloc;
        }
    }

malloc: // Once we have found the right place
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
