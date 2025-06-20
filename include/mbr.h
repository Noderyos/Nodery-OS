#ifndef MBR_H
#define MBR_H

#include "types.h"

struct __attribute__((packed)) mbr {
    u8 code[446];
    struct __attribute__((packed)) {
        u8 status;
        u8 chs_start[3];
        u8 type;
        u8 chs_end[3];
        u32 lba_start;
        u32 lba_size;
    } parts[4];
};

extern struct mbr *mbr;

#endif
