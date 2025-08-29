#include "fat.h"
#include "sys/lba.h"
#include "io.h"
#include "malloc.h"
#include "string.h"

#define MIN(x,y) ((x)<(y)?(x):(y))
#define MAX(x,y) ((x)>(y)?(x):(y))

struct fat_bootsector bs;

struct fat_attr {
    uint8_t ReadOnly   : 1;
    uint8_t Hidden     : 1;
    uint8_t System     : 1;
    uint8_t VolumeName : 1;
    uint8_t Directory  : 1;
    uint8_t AchieveFlag: 1;
    uint8_t Reserved   : 2;
};

struct Date {
    uint8_t Day: 5;
    uint8_t Month: 4;
    uint8_t Year: 7;
};

struct Time {
    uint8_t Second: 5;
    uint8_t Minute: 6;
    uint8_t Hour: 5;
};

struct fat_dir_entry {
    char filename[8];
    char extension[3];
    struct fat_attr attr;
    uint8_t resv;
    uint8_t c_millis;
    struct Time c_time;
    struct Date c_date;
    struct Date a_date;
    uint16_t resv2;
    struct Time w_time;
    struct Date w_date;
    uint16_t cluster;
    uint32_t filesize;
};

uint16_t *fat;

struct fat_dir_entry *root_entries;

uint32_t data_addr;

uint32_t bytes_per_cluster;

void clean_entry(struct fat_dir_entry *entry) {
    for (int i = 0; i < 8; i++)
        if (entry->filename[i] == ' ') entry->filename[i] = 0;
    for (int i = 0; i < 3; i++)
        if (entry->extension[i] == ' ') entry->extension[i] = 0;
}

int init_fs(uint32_t start_sector) {
    ata_lba_read(start_sector, 1, &bs);
    if (!bs.code_jump[0]) return -1;
    if (bs.bytes_per_sector != 512) return -2;

    uint32_t fat_addr = start_sector + bs.resv_sectors;
    fat = malloc(bs.sectors_per_fat * bs.bytes_per_sector);
    ata_lba_read(fat_addr, bs.sectors_per_fat, fat);

    uint32_t root_entries_addr = fat_addr + bs.sectors_per_fat * bs.fat_count;
    root_entries = malloc(bs.root_entry_count * sizeof(struct fat_dir_entry));
    ata_lba_read(root_entries_addr, (bs.root_entry_count * sizeof(struct fat_dir_entry))/512, root_entries);

    data_addr = root_entries_addr + (bs.root_entry_count * sizeof(struct fat_dir_entry))/512;
    bytes_per_cluster = bs.sectors_per_cluster * bs.bytes_per_sector;
    for (int i = 0; i < bs.root_entry_count; i++) clean_entry(&root_entries[i]);

    return 0;
}


void format_entry(struct fat_dir_entry entry, char buf[13]) {
    memset(buf, 0, 13);
    strncpy(buf, entry.filename, 8);
    if (!entry.extension[0]) return;
    strcat(buf, ".");
    strncat(buf, entry.extension, 3);
}

int read_file(char *filename) {
    char fname[13];
    int idx = -1;
    for (int i = 0; i < 5; i++) {
        struct fat_dir_entry entry = root_entries[i];
        format_entry(entry, fname);
        
        if (strcmp(fname, filename) == 0 && !entry.attr.Directory) {
            idx  = i;
            break;
        }
    }
    if (idx < 0) return -1;

    uint16_t cur_cluster = root_entries[idx].cluster;
    uint32_t filesize = root_entries[idx].filesize;

    uint8_t *buf = malloc(bs.sectors_per_cluster * bs.bytes_per_sector);
    if (!buf) return -1;

    while (cur_cluster != 0xFFFF) {
        ata_lba_read(data_addr + (cur_cluster-2)*bs.sectors_per_cluster, bs.sectors_per_cluster, buf);
        uint32_t to_read = filesize > bytes_per_cluster ? bytes_per_cluster : filesize;
        
        for (uint32_t i = 0; i < to_read; i++) printf("%c", buf[i]);
        
        cur_cluster = fat[cur_cluster];
        filesize -= bytes_per_cluster;
    }

    free(buf);
    return 0;
}

void format_attrs(struct fat_attr attrs, char *buf) {
    buf[0] = attrs.ReadOnly ? 'R' : '-';
    buf[1] = attrs.Hidden   ? 'H' : '-';
    buf[2] = attrs.System   ? 'S' : '-';
    buf[3] = attrs.VolumeName ? 'V': '-';
    buf[4] = attrs.Directory  ? 'D' : '-';
    buf[5] = attrs.AchieveFlag ? 'A': '-';
    buf[6] = attrs.Reserved &0b01 ? 'x' : '-';
    buf[7] = attrs.Reserved &0b10 ? 'y' : '-';
}

int list_dir() {
    char attrs[9];
    for (int i = 0; i < bs.root_entry_count; i++) {
        struct fat_dir_entry entry = root_entries[i];
        if (entry.filename[0] == 0) break;
        format_attrs(entry.attr, attrs);

        printf(
                entry.attr.Directory || *entry.extension == 0
                    ? "%s %d-%d-%d %d:%d:%d %s\n"
                    : "%s %d-%d-%d %d:%d:%d %s.%s\n",
                attrs,
                entry.c_date.Year + 1980, entry.c_date.Month, entry.c_date.Day,
                entry.c_time.Hour, entry.c_time.Minute, entry.c_time.Second*2, 
                entry.filename, entry.extension);
 
    }
    return 0;
}

FILE *fopen(char *path, char *mode) {
    (void)mode;

    char fname[13];
    int idx = -1;
    for (int i = 0; i < 5; i++) {
        struct fat_dir_entry entry = root_entries[i];
        format_entry(entry, fname);
        
        if (strcmp(fname, path) == 0 && !entry.attr.Directory) {
            idx  = i;
            break;
        }
    }
    if (idx < 0) return 0;

    FILE *file = malloc(sizeof(FILE));
    file->cluster = root_entries[idx].cluster;
    file->filesize = root_entries[idx].filesize;
    file->offset = 0;

    return file;
}

uint16_t get_nth_cluster(uint16_t start, uint16_t count) {
    uint16_t cur_cluster = start;
    while (count--) {
        if (count && cur_cluster == 0xFFFF) return 0xFFFF;
        cur_cluster = fat[cur_cluster];
    }
    return cur_cluster;
}

uint32_t fread(void *buf, uint32_t size, uint32_t n, FILE *stream) {
    uint8_t *buffer = malloc(bytes_per_cluster);

    uint32_t to_read = n * size;

    uint16_t cur_cluster = get_nth_cluster(stream->cluster, stream->offset / bytes_per_cluster);
    uint32_t offset = stream->offset % bytes_per_cluster;

    while (to_read) {
        if (stream->filesize == stream->offset) break;
        uint32_t r = MIN(to_read, bytes_per_cluster-offset);
        r = MIN(r, stream->filesize-stream->offset);
        ata_lba_read(data_addr + (cur_cluster-2)*bs.sectors_per_cluster, bs.sectors_per_cluster, buffer);
        memcpy(buf, buffer+offset, r);
        buf += r;
        to_read -= r;
        stream->offset += r;
        offset = (offset+r)%bytes_per_cluster;
        cur_cluster = fat[cur_cluster];
    }
    free(buffer);
    return n*size - to_read;
}

int fseek(FILE *stream, long offset, int whence) {
    switch (whence) {
        case SEEK_SET:
            stream->offset = offset;
            break;
        case SEEK_CUR:
            stream->offset += offset;
            break;
        case SEEK_END:
            stream->offset = stream->filesize - offset;
            break;
        default: return -1;
    }
    return 0;
}

int fclose(FILE *stream) {
    return free(stream);
}

uint16_t allocate_cluster(uint16_t current) {
    for (uint32_t i = 1; i < bs.sectors_per_fat*bs.bytes_per_sector/sizeof(*fat); i++) {
        if (fat[i] == 0x0000) {
            fat[current] = i;
            fat[i] = 0xFFFF;
            return i;
        }
    }
    return 0;
}

uint32_t fwrite(void *ptr, uint32_t size, uint32_t n, FILE *stream) {
    uint8_t *buffer = malloc(bytes_per_cluster);    

    uint32_t to_write = n * size;

    uint16_t cur_cluster = get_nth_cluster(stream->cluster, stream->offset / bytes_per_cluster);
    uint32_t offset = stream->offset % bytes_per_cluster;

    while (to_write) {
        uint32_t r = MIN(to_write, bytes_per_cluster-offset);

        ata_lba_read(data_addr + (cur_cluster-2)*bs.sectors_per_cluster, bs.sectors_per_cluster, buffer);
        memcpy(buffer + offset, ptr, r);
        ata_lba_write(data_addr + (cur_cluster-2)*bs.sectors_per_cluster, bs.sectors_per_cluster, buffer);
        ptr += r;
        to_write -= r;
        stream->offset += r;
        offset = (offset+r)%bytes_per_cluster;
        if (fat[cur_cluster] == 0xFFFF && to_write) {
            cur_cluster = allocate_cluster(cur_cluster);
            if (!cur_cluster) break;
        }
    }
    free(buffer);
    if (stream->offset > stream->filesize) stream->filesize = stream->offset;
    return n*size - to_write; 
}
