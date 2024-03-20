#include "asm/ata.h"

void read_sectors(u32 start, u8 count, u8 *buf){
    for (int i = 0; i < count; ++i) {
        _ata_lba_read(start, 1, buf);
        start++;
        buf += 512;
    }
}

void write_sectors(u32 start, u8 count, u8 *buf){
    for (int i = 0; i < count; ++i) {
        _ata_lba_write(start, 1, buf);
        start++;
        buf += 512;
    }
}