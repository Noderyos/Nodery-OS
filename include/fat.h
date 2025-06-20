#ifndef FAT_H
#define FAT_H

#include "types.h"

typedef enum : u8 {
    disk = 0xF8
} MediaDescriptor;

struct __attribute__((packed)) fat_bootsector {
    u8 code_jump[3];
    char oem_id[8];
    u16 bytes_per_sector;
    u8 sectors_per_cluster;
    u16 resv_sectors;
    u8 fat_count;
    u16 root_entry_count;
    u16 smol_sector_count;
    MediaDescriptor media_descriptor;
    u16 sectors_per_fat;
    u16 sectors_per_track;
    u16 head_count;
    u32 hidden_sectors;
    u32 large_sector_count;
    u8 drive_number;
    u8 resv;
    u8 ext_boot_sig;
    u32 serial_number;
    char label[11];
    char fs_type[8];
    u8 code[448];
    u16 sig;
};

typedef struct FILE {
    u16 cluster;
    u32 filesize;
    u32 offset;
} FILE;

enum {
    SEEK_SET, SEEK_CUR, SEEK_END
};

int init_fs(u32 start_sector);
int read_file(char *filename);
int list_dir();

u32 fread(void *buf, u32 size, u32 n, FILE *stream);
FILE *fopen(char *path, char *mode);
int fseek(FILE *stream, long offset, int whence);
int fclose(FILE *stream);
u32 fwrite(void *ptr, u32 size, u32 n, FILE *stream);

#endif
