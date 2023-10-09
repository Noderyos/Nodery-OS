#ifndef ATA_H
#define ATA_H

#include "types.h"


extern void ata_lba_read(u32 sector, u8 count, u8 *buf);
extern void ata_lba_write(u32 sector, u8 count, u8 *buf);

#endif
