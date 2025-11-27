#include "elf.h"

#include "fat.h"
#include "io.h"
#include "string.h"
#include "malloc.h"
#include "mmap.h"
#include "scheduler.h"

#include "sys/vga.h"

#define ERROR(x) \
    do { \
        setColor(RED); \
        printf("[ERROR] %s\n", x); \
    } while(0)

int parse_elf_header(FILE *f, Elf32_Ehdr *elf_hdr) {
    if (f == NULL) {
        ERROR("Failed to open file");
        return -1;
    }

    fread(elf_hdr, sizeof(*elf_hdr), 1, f);

    if (elf_hdr->e_ident[0] != 0x7f 
        || elf_hdr->e_ident[1] != 'E'
        || elf_hdr->e_ident[2] != 'L'
        || elf_hdr->e_ident[3] != 'F') {
        ERROR("Not an ELF file");
        return -1;
    }
    
    if (elf_hdr->e_ident[4] != ELFCLASS32) {
        ERROR("Invalid ELF wordsize");
        return -1;
    }

    if (elf_hdr->e_ident[5] != ELFDATA2LSB) {
        ERROR("Invalid ELF endianness");
        return -1;
    }

    if (elf_hdr->e_ident[6] != EV_CURRENT) {
        ERROR("Invalid ELF header version");
        return -1;
    }

    if (elf_hdr->e_version != EV_CURRENT) {
        ERROR("Invalid ELF version");
        return -1;
    }

    if (elf_hdr->e_type != ET_EXEC) {
        ERROR("Invalid ELF type");
        return -1;
    }

    if (elf_hdr->e_machine != EM_386) {
        ERROR("Invalid ELF machine");
        return -1;
    }

    if (elf_hdr->e_shentsize != sizeof(Elf32_Shdr)) {
        ERROR("Invalid ELF section header size");
        return -1;
    }
    if (elf_hdr->e_phentsize != sizeof(Elf32_Phdr)) {
        ERROR("Invalid ELF program header size");
        return -1;
    }
    return 0;
}

char *load_str_table(FILE *f, Elf32_Ehdr elf_hdr) {
    fseek(f, elf_hdr.e_shoff + elf_hdr.e_shstrndx*sizeof(Elf32_Shdr), SEEK_SET);
    
    Elf32_Shdr str_hdr = {0};
    fread(&str_hdr, sizeof(str_hdr), 1, f);

    if (str_hdr.sh_type != SHT_STRTAB) {
        ERROR("Invalid string table header");
        return NULL;
    }

    char *str_table = malloc(str_hdr.sh_size);
    fseek(f, str_hdr.sh_offset, SEEK_SET);
    fread(str_table, 1, str_hdr.sh_size, f);

    return str_table;
}

int load_elf(char *filename) {
    FILE *f = fopen(filename, "r");

    Elf32_Ehdr elf_hdr = {0};
    if (parse_elf_header(f, &elf_hdr) < 0) return -1;

    uint32_t *task_pd_phys = create_pd();
    uint32_t *task_pd = (uint32_t*)0xF0000000;
    if (map_page(PD_ADDR, task_pd_phys, task_pd, PAGE_DEFAULT) < 0) return -1;
    
    fseek(f, elf_hdr.e_phoff, SEEK_SET);
    for (int p_idx = 0; p_idx < elf_hdr.e_phnum; p_idx++) {
        Elf32_Phdr prog = {0};
        fseek(f, elf_hdr.e_phoff + p_idx*sizeof(Elf32_Phdr), SEEK_SET);
        fread(&prog, sizeof(prog), 1, f);

        printf("[DEBUG] Ptype = %d, Virt %x, Fsize %d, Msize %d, Align %x\n", 
                prog.p_type, prog.p_vaddr, prog.p_filesz, prog.p_memsz, prog.p_align);        
        
        if (prog.p_type == PT_NULL) continue;
        if (prog.p_type == PT_DYNAMIC || prog.p_type == PT_INTERP) {
            ERROR("Cannot load dynamic ELF");
            return -1;
        }

        if (prog.p_type != PT_LOAD) continue;

        if (prog.p_align != PAGE_SIZE) {
            ERROR("ELF is not aliged on PAGE_SIZE");
            return -1;
        }
        if ((prog.p_vaddr & (PAGE_SIZE-1)) != prog.p_offset) {
            ERROR("Invalid offset in program header");
            return -1;
        }

        uint32_t virt_addr = prog.p_vaddr & ~(PAGE_SIZE-1);
        uint32_t p_len = prog.p_offset + prog.p_memsz;
        if (mmap(PD_ADDR, (void*)virt_addr, p_len, PAGE_USER | PAGE_RW) == NULL ||
            mmap(task_pd, (void*)virt_addr, p_len, PAGE_USER | PAGE_RW) == NULL) {
            ERROR("Cannot mmap");
            return -1;
        }

        if (prog.p_filesz) {
            fseek(f, prog.p_offset, SEEK_SET);
            fread((void *)prog.p_vaddr, 1, prog.p_filesz, f);
        } else {
            memset((void *)prog.p_vaddr, 0, prog.p_memsz);
        }


        for (uint32_t da = 0; da <= p_len; da += PAGE_SIZE) {
            uint32_t addr = virt_addr + da;
            uint32_t pd_index = (addr >> 22) & 0x3FF;
            uint32_t pt_index = (addr >> 12) & 0x3FF;
 
            uint32_t *pt_phys = (uint32_t*)task_pd[pd_index];
            uint32_t *pt = (uint32_t*)0xF0002000;
            if (map_page(PD_ADDR, pt_phys, pt, PAGE_DEFAULT) < 0) return -1;

            void *page = (void*)0xF0003000;
            if (map_page(PD_ADDR, (uint32_t *)(pt[pt_index] & 0xFFFFF000), page, PAGE_DEFAULT) < 0) return -1;
            memcpy((uint32_t*)0xF0003000, (uint32_t*)addr, PAGE_SIZE);
        }
    }

    if (create_task(task_pd_phys, (void*)elf_hdr.e_entry) < 0) return -1;

    fclose(f);
    return 0;
}
