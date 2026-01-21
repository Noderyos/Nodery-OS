#ifndef FAT_H
#define FAT_H

#include "types.h"

typedef enum : uint8_t {
    disk = 0xF8
} MediaDescriptor;

struct fat_bootsector {
    uint8_t code_jump[3];
    char oem_id[8];
    uint16_t bytes_per_sector;
    uint8_t sectors_per_cluster;
    uint16_t resv_sectors;
    uint8_t fat_count;
    uint16_t root_entry_count;
    uint16_t smol_sector_count;
    MediaDescriptor media_descriptor;
    uint16_t sectors_per_fat;
    uint16_t sectors_per_track;
    uint16_t head_count;
    uint32_t hidden_sectors;
    uint32_t large_sector_count;
    uint8_t drive_number;
    uint8_t resv;
    uint8_t ext_boot_sig;
    uint32_t serial_number;
    char label[11];
    char fs_type[8];
    uint8_t code[448];
    uint16_t sig;
};

typedef struct FILE {
    uint16_t cluster;
    uint32_t filesize;
    uint32_t offset;
} FILE;

enum {
    SEEK_SET, SEEK_CUR, SEEK_END
};

typedef struct folder {
    uint32_t depth;
    uint32_t cluster;
} folder_t;

int init_fs(uint32_t start_sector);

int chdir(char *dirname);
uint32_t fread(void *buf, uint32_t size, uint32_t n, FILE *stream);
FILE *fopen(char *path, char *mode);
int fseek(FILE *stream, long offset, int whence);
int fclose(FILE *stream);
uint32_t fwrite(void *ptr, uint32_t size, uint32_t n, FILE *stream);

#endif
