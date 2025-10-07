#ifndef SYSCALLS_H
#define SYSCALLS_H

#include "types.h"

typedef struct regs {
    uint32_t edi,esi,ebp,esp,ebx,edx,ecx,eax;
} regs_t;
uint32_t handle_syscall(regs_t *r);
int setup_fds();
#endif
