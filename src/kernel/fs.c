#include "fs.h"
#include "kernel/malloc.h"

MBR *mbr = (void*)0x7c00;

FAT_Hdr fat_hdr;
u16 *fat;
FAT_Entry *root_entries;
u32 partition_start, fat_start, root_entry_start, data_start;


void cleanup_names(FAT_Entry *entries, int max_entries){
    for (int i = 0; i < max_entries; ++i) {
        FAT_Entry *entry = &entries[i];
        if(entry->filename[0] == 0) continue;
        for (int j = 0; j < 9; ++j)
            if(entry->filename[j] == ' ')
                entry->filename[j] = 0;
        for (int j = 0; j < 3; ++j)
            if(entry->ext[j] == ' ')
                entry->ext[j] = 0;

    }
}

enum {
    FAT_READ_ONLY    = 0x01,
    FAT_HIDDEN       = 0x02,
    FAT_SYSTEM       = 0x04,
    FAT_VOLUME_NAME  = 0x08,
    FAT_DIRECTORY    = 0x10,
    FAT_ACHIEVE      = 0x20
};

void format_attrs(char* perms, u8 attrs){
    if(attrs & FAT_VOLUME_NAME) perms[0] = 'v';
    if(attrs & FAT_DIRECTORY) perms[1] = 'd';
    if(attrs & FAT_SYSTEM) perms[2] = 's';
    if(attrs & FAT_HIDDEN) perms[3] = 'h';
    if(attrs & FAT_READ_ONLY) perms[4] = 'r';
    if(attrs & FAT_ACHIEVE) perms[5] = 'a';
}

int init_fs(int p_number){
    if(mbr->signature != 0xAA55) return -1;

    MBR_Entry part = mbr->entries[p_number];
    partition_start = part.lba_addr;

    read_sectors(partition_start, 1, &fat_hdr);

    if(strncmp(fat_hdr.type, "FAT16   ", 8) != 0){
        return -1;
    }

    fat_start = partition_start + fat_hdr.reserved_sectors;
    root_entry_start = fat_start + fat_hdr.fat_copies_count*fat_hdr.sectors_per_fat;
    data_start = root_entry_start + fat_hdr.root_entries_count*32/512;

    fat = malloc(fat_hdr.sectors_per_fat * 512);
    read_sectors(fat_start, fat_hdr.sectors_per_fat, fat);

    root_entries = malloc(fat_hdr.root_entries_count*32);

    read_sectors(root_entry_start, fat_hdr.root_entries_count/512, root_entries);
    cleanup_names(root_entries, fat_hdr.root_entries_count);

    return 0;
}

void list_files(){
    for (int i = 0; i < fat_hdr.root_entries_count; ++i){
        FAT_Entry entry = root_entries[i];
        if(entry.filename[0] == 0) continue;
        char perms[7] = "------";
        format_attrs(perms, entry.attr);
        if(entry.attr & FAT_DIRECTORY)
            print(0x7F7FFF, "%s  %s\n", perms,entry.filename);
        else
            print(0xFFFFFF, "%s %d %s.%s\n", perms, entry.size, entry.filename, entry.ext);
    }
}

void memcpy(char *dst, char *src, u32 count){
    for (u32 i = 0; i < count; ++i) {
        dst[i] = src[i];
    }
}


void *read_file(char *filename){

    char* ext = strstr(filename, ".");

    if(ext == 0)
        return 0;


    char *name = malloc(ext-filename);
    memcpy(name, filename, ext-filename);
    ext++;  // Move after '.'

    FAT_Entry file = {0};
    for (int i = 0; i < fat_hdr.root_entries_count; ++i) {

        // TODO : Fix name comparison, if name start with and ext start with, then it validates
        if(strncmp(root_entries[i].filename, name, 8) == 0 &&
                strncmp(root_entries[i].ext, ext, 8) == 0){
            file = root_entries[i];
            break;
        }
    }
    free(name);

    if(file.filename[0] == 0) return 0;

    u8* ptr = malloc((file.size/512+1)*512);
    u8* content = ptr;

    u16 fat_idx = file.cluster;

    do{
        read_sectors(data_start + (fat_idx-2)*fat_hdr.sector_per_cluster,
                     fat_hdr.sector_per_cluster,content);
        content += fat_hdr.sector_per_cluster*512;
        fat_idx = fat[fat_idx];
    } while (fat_idx < 0xFFF7);

    return ptr;
}