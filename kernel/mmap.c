#include "mmap.h"
#include "types.h"
#include "string.h"
#include "sys/vga.h"
#include "io.h"

uint32_t MAX_PAGES;

uint8_t *memory_bitmap = (uint8_t *)0x60000;
uint32_t *page_directory = (uint32_t *)0x80000;

static uint32_t current_idx = 0;

void set_page_used(uint32_t page_idx) {
    memory_bitmap[page_idx/8] |= 1<<(page_idx%8);
    current_idx = page_idx/8;
}

int is_page_used(uint32_t page_idx) {
    return memory_bitmap[page_idx/8] & 1<<(page_idx%8);
}


void *alloc_page() {
    for(uint32_t i = current_idx*8; i < MAX_PAGES; i++) {
        if (!is_page_used(i)) {
            set_page_used(i);
            return (void *)(ALLOC_START + i*PAGE_SIZE);
        }
    }
    return 0;
}


uint32_t *get_page_directory() {
    return (uint32_t*)PD_ADDR;
}

uint32_t *get_page_table(uint32_t *pd, uint16_t page_idx) {
    if (pd == PD_ADDR) {
        return (uint32_t *)(((uint32_t)PT_ADDR) + page_idx * PAGE_SIZE);
    }
    else {
        if (map_page(PD_ADDR, (void*)(pd[page_idx]&0xFFFFF000), (void*)0xF0001000, PAGE_DEFAULT) < 0) return NULL;
        return (uint32_t*)0xF0001000;
    }
}

int map_page(uint32_t *pd, void *phys_addr, void *virt_addr, uint32_t flags) {
    uint32_t pd_index = ((uint32_t)virt_addr >> 22) & 0x3FF;
    uint32_t pt_index = ((uint32_t)virt_addr >> 12) & 0x3FF; 
    uint32_t *pt;

    if (!(pd[pd_index] & PAGE_PRESENT)) {
        uint32_t *phys = alloc_page();
        if (!phys) return -1;
        pd[pd_index] = (uint32_t)phys | (flags & 0xFFF);
        pt = get_page_table(pd, pd_index);
        memset(pt, 0, PAGE_SIZE);
    } else {
        pt = get_page_table(pd, pd_index);
    }

    if (pt == NULL) return -1;

    pt[pt_index] = ((uint32_t)phys_addr & 0xFFFFF000) | (flags & 0xFFF);
 
    if (pd == PD_ADDR) __asm__ volatile("invlpg (%0)" :: "r"(virt_addr) : "memory");
    return 0;
}

void *mmap(uint32_t *pd, void *virt_addr, uint32_t len, int flags) {
    for (uint32_t i = 0; i < len; i+= PAGE_SIZE) {
        void *phys = alloc_page();
        if (!phys) return NULL;
        if (map_page(pd, phys, virt_addr + i, flags | PAGE_PRESENT) < 0) return NULL;
    }
    return (void *)virt_addr;
}

int init_paging(uint32_t available_memory) {
    MAX_PAGES = (available_memory-ALLOC_START)/PAGE_SIZE;
    memset(page_directory, 0, PAGE_SIZE);
    memset(memory_bitmap, 0, 0x20000);

    uint32_t *first_pt = (uint32_t *)alloc_page();
    if (!first_pt) return -1;
    memset(first_pt, 0, PAGE_SIZE);
    
    for (uint32_t i = 0; i < 1024; i++)
        first_pt[i] = (i * PAGE_SIZE) | PAGE_DEFAULT;

    page_directory[0] = (uint32_t)first_pt | PAGE_DEFAULT;
    page_directory[1023] = (uint32_t)page_directory | PAGE_DEFAULT;
    // Load page_directory
    __asm__ volatile("mov %0, %%cr3" :: "r"(page_directory));

    // Enable paging
    uint32_t cr0;
    __asm__ volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000;
    __asm__ volatile("mov %0, %%cr0" :: "r"(cr0));
    
    for (uint32_t i = 0; i < vbe->width*vbe->height*(vbe->bpp/8); i += PAGE_SIZE) {
        void* a = (void*)((uint32_t)vbe->framebuffer&0xFFFFF000) + i;
        if(map_page(PD_ADDR, a, a, PAGE_DEFAULT) < 0) return -1;
    } 

    for (int i = 0; i < 0x10000000; i += PAGE_SIZE) {
        void *p = alloc_page();
        map_page(PD_ADDR, p, (void*)(0xE0000000+i), PAGE_DEFAULT);
    }

    return 0;
}
