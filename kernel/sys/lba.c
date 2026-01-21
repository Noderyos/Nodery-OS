#include "sys/lba.h"

#define ATA_PRIMARY 0x1F0

void ata_lba_read(uint32_t sector, uint8_t count, void *buf) {
    asm("cli");
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
            *(uint16_t*)buf = inw(ATA_PRIMARY);
        }
    }
    asm("sti");
}

void ata_lba_write(uint32_t sector, uint8_t count, void *buf) {
    asm("cli");
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
            outw(ATA_PRIMARY, *(uint16_t*)buf);
        }
    }
    asm("sti");
}

