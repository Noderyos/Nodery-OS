#ifndef FS_H
#define FS_H

#include "types.h"
#include "string.h"
#include "asm/ata.h"
#include "kernel/io.h"

// MBR related

typedef struct {
    u8 status;
    u8 chs_first[3];
    u8 type;
    u8 chs_end[3];
    u32 lba_addr;
    u32 sect_count;
}__attribute__((packed)) MBR_Entry;

typedef struct {
    u8 bootloader[446];
    MBR_Entry entries[4];
    u16 signature;
}__attribute__((packed)) MBR;

// FAT Related

typedef struct {
    u8 code[3];
    u8 oem_id[8];
    u16 bytes_per_sector;
    u8 sector_per_cluster;
    u16 reserved_sectors;
    u8 fat_copies_count;
    u16 root_entries_count;
    u16 smol_sectors_count;
    u8 media_descriptor;
    u16 sectors_per_fat;
    u16 sectors_per_track;
    u16 heads_count;
    u32 hidden_sectors;
    u32 large_sectors_count;
    u8 drive_number;
    u8 reserved;
    u8 extended_boot_signature;
    u32 volume_serial_number;
    u8 label[11];
    u8 type[8];
    u8 bootstrap[448];
    u16 signature;
} __attribute((packed)) FAT_Hdr;

typedef struct {
    u8 filename[8];
    u8 ext[3];
    u8 attr;
    u8 reserved;
    u8 c_100th_sec;
    u16 c_time;
    u16 c_date;
    u16 a_date;
    u16 f32_reserved;
    u16 w_time;
    u16 w_date;
    u16 cluster;
    u32 size;
} __attribute((packed)) FAT_Entry;

int init_fs(int p_number);
void list_files();
void *read_file(char *filename);

#endif
