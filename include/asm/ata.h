#ifndef ATA_H
#define ATA_H

#include "types.h"


extern void _ata_lba_read(u32 sector, u8 count, u8 *buf);
extern void _ata_lba_write(u32 sector, u8 count, u8 *buf);

void read_sectors(u32 start, u8 count, u8 *buf);
void write_sectors(u32 start, u8 count, u8 *buf);

#endif
