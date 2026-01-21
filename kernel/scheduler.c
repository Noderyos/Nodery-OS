#include "scheduler.h"

#include "types.h"
#include "mmap.h"
#include "sys/vga.h"
#include "string.h"

struct task tasks[MAX_TASKS];

uint32_t cur_task_idx = 0;
struct task *current_task;

void exit_task() {
    tasks[cur_task_idx].present = 0;
}

int init_multitasking() {
    tasks[0] = (struct task){.esp = 0, .cr3 = 0x80000, .present = 1};
    tasks[0].cwd = (folder_t){.depth = 0, .cluster = 0};
    cur_task_idx = 0;
    current_task = &tasks[cur_task_idx];
    return 0;
}

void scheduler() {
    do {
        cur_task_idx = (cur_task_idx + 1) % MAX_TASKS;
    } while(tasks[cur_task_idx].present == 0);
    current_task = &tasks[cur_task_idx];
}

uint32_t *create_pd() {
    void *pd_phys = alloc_page();
    if (!pd_phys) return NULL;

    uint32_t *pd = (uint32_t*)0xF0000000;
    if(map_page(PD_ADDR, pd_phys, pd, PAGE_DEFAULT) < 0) return NULL;
    memset(pd, 0, PAGE_SIZE);


    uint32_t *pt_phys = alloc_page();
    if (!pt_phys) return NULL;

    uint32_t *pt = (uint32_t*)0xF0001000;
    if (map_page(PD_ADDR, pt_phys, pt, PAGE_DEFAULT) < 0)
        return NULL;
    
    memset(pt, 0, PAGE_SIZE);
    
    for (uint32_t i = 0; i < 1024; i++)
        pt[i] = (i * PAGE_SIZE) | PAGE_DEFAULT;

    pd[0] = (uint32_t)pt_phys | PAGE_DEFAULT;
    pd[1023] = (uint32_t)pd_phys | PAGE_DEFAULT;

    for (uint32_t i = 0; i < vbe->width*vbe->height*(vbe->bpp/8); i += PAGE_SIZE) {
        void *a = (void*)((uint32_t)vbe->framebuffer&0xFFFFF000) + i;
        if(map_page(pd, a, a, PAGE_DEFAULT) < 0) return NULL;
    } 

    for (uint32_t i = 0; i < 0x1000000/PAGE_SIZE; i++) {
        uint32_t virt_addr = 0xE0000000 + i*PAGE_SIZE;
        uint32_t pd_index = (virt_addr >> 22) & 0x3FF;
        uint32_t pt_index = (virt_addr >> 12) & 0x3FF; 

        uint32_t *pt = (uint32_t *)(((uint32_t)PT_ADDR) + pd_index * PAGE_SIZE);
        if (map_page(pd, (void*)(pt[pt_index]&0xFFFFF000), (void*)virt_addr, PAGE_DEFAULT) < 0) return NULL;
    }
    return pd_phys;
}

int create_task(uint32_t *pd_phys, void *entry) {
    uint32_t *pd = (uint32_t*)0xF0000000;
    if (map_page(PD_ADDR, pd_phys, pd, PAGE_DEFAULT) < 0) return -1;
    
    void *stack_phys = alloc_page();
    if (map_page(PD_ADDR, stack_phys, (void *)0xF0004000, PAGE_DEFAULT) < 0) return -1;
    if (map_page(pd, stack_phys, (void *)0xD0000000, PAGE_DEFAULT)) return -1;

    struct task elf_task;

    elf_task.present = 1;
    elf_task.cr3 = (uint32_t)pd_phys;
    elf_task.esp = 0xD0000000+PAGE_SIZE;

    uint32_t *stack = (uint32_t *)(0xF0004000 + PAGE_SIZE);
   
    // iretd structure
    *(--stack) = 0x202; // EFLAGS = 0b10 | IF
    *(--stack) = 0x8; // CS
    *(--stack) = (uint32_t)entry; // EIP

    // popa structure
    *(--stack) = 0; // EAX
    *(--stack) = 0; // ECX
    *(--stack) = 0; // EDX
    *(--stack) = 0; // EBX
    *(--stack) = 0xD0000000; // ESP
    *(--stack) = 0xD0000000; // EBP
    *(--stack) = 0; // ESI
    *(--stack) = 0; // EDI
    
    elf_task.esp -= 11*4;
    
    elf_task.cwd = current_task->cwd;

    for (uint32_t i = 0; i < MAX_TASKS; i++) {
        if (!tasks[i].present) {
            tasks[i] = elf_task;
            return 0;
        }
    }

    return -1;
}
