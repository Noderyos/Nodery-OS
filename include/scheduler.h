#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "types.h"
#include "fat.h"

extern struct task *current_task;

struct task {
    uint32_t esp;
    uint32_t cr3;
    folder_t cwd;
    int present;
};

#define MAX_TASKS 16

void exit_task();
int init_multitasking();
uint32_t *create_pd();
int create_task(uint32_t *pd_phys, void *entry);

#endif
