#ifndef MBR_H
#define MBR_H

#include "types.h"

struct __attribute__((packed)) mbr {
    uint8_t code[446];
    struct __attribute__((packed)) {
        uint8_t status;
        uint8_t chs_start[3];
        uint8_t type;
        uint8_t chs_end[3];
        uint32_t lba_start;
        uint32_t lba_size;
    } parts[4];
};

extern struct mbr *mbr;

#endif
