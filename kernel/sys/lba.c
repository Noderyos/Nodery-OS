#include "sys/lba.h"

#define ATA_PRIMARY 0x1F0

void ata_lba_read(u32 sector, u8 count, void *buf) {


    
    /*
Send a NULL byte to port 0x1F1, if you like (it is ignored and wastes lots of CPU time): outb(0x1F1, 0x00)
Send the sectorcount to port 0x1F2: outb(0x1F2, (unsigned char) count)
Send the low 8 bits of the LBA to port 0x1F3: outb(0x1F3, (unsigned char) LBA))
Send the next 8 bits of the LBA to port 0x1F4: outb(0x1F4, (unsigned char)(LBA >> 8))
Send the next 8 bits of the LBA to port 0x1F5: outb(0x1F5, (unsigned char)(LBA >> 16))
Send the "READ SECTORS" command (0x20) to port 0x1F7: outb(0x1F7, 0x20)
   */

    while(inb(ATA_PRIMARY+7) & 0x80);

    outb(ATA_PRIMARY+1, 0x00);
    outb(ATA_PRIMARY+2, count);
    outb(ATA_PRIMARY+3, sector&0xFF);
    outb(ATA_PRIMARY+4, (sector>>8)&0xFF);
    outb(ATA_PRIMARY+5, (sector>>16)&0xFF);
    outb(ATA_PRIMARY+6, 0xE0 | ((sector>>24)&0x0F));
    outb(ATA_PRIMARY+7, 0x20);

    for(int a = 0; a < count; a++) {
        while(inb(ATA_PRIMARY+7) & 0x80);
        for(char *end = buf + 512; buf != end; buf+=2) {
            *(u16*)buf = inw(ATA_PRIMARY);
        }
    }
}

void ata_lba_write(u32 sector, u8 count, void *buf) {
    while(inb(ATA_PRIMARY+7) & 0x80);

    outb(ATA_PRIMARY+1, 0x00);
    outb(ATA_PRIMARY+2, count);
    outb(ATA_PRIMARY+3, sector&0xFF);
    outb(ATA_PRIMARY+4, (sector>>8)&0xFF);
    outb(ATA_PRIMARY+5, (sector>>16)&0xFF);
    outb(ATA_PRIMARY+6, 0xE0 | ((sector>>24)&0x0F));
    outb(ATA_PRIMARY+7, 0x30);

    for(int a = 0; a < count; a++) {
        while(inb(ATA_PRIMARY+7) & 0x80);
        for(char *end = buf + 512; buf != end; buf+=2) {
            outw(ATA_PRIMARY, *(u16*)buf);
        }
    }
}

