#ifndef LBA_H
#define LBA_H

#include "types.h"
#include "../io.h"
#include "ports.h"

void ata_lba_read(u32 sector, u8 count, void *buf);
void ata_lba_write(u32 sector, u8 count, void *buf);

#endif
