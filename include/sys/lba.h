#ifndef LBA_H
#define LBA_H

#include "types.h"
#include "../io.h"
#include "ports.h"

void ata_lba_read(uint32_t sector, uint8_t count, void *buf);
void ata_lba_write(uint32_t sector, uint8_t count, void *buf);

#endif
