#include "syscalls.h"
#include "io.h"

#include "sys/ports.h"
#include "sys/ps2.h"
#include "sys/keymap.h"
#include "fat.h"
#include "mmap.h"

int is_shift = 0;

int enter_pressed = 0;
char *read_buf = 0;
uint32_t read_len = 0;
uint32_t read_idx = 0;

void handle_keyboard() {
    uint8_t status = inb(PS2_COMMAND);
    if (status & 1) {
        uint8_t keycode = inb(PS2_DATA);
        if(keycode == LSHIFT) is_shift = 1;
        else if(keycode == LSHIFT+0x80) is_shift = 0;
        else if(keycode == LMAJ) is_shift = !is_shift;
        else if(keycode == BACKSPACE) {
            backspace();
            if (read_buf && read_idx) read_buf[--read_idx] = 0;    
        }
        else if(keycode == ENTER) {
            putchar('\n');
            if(read_buf && read_idx < read_len) read_buf[read_idx++] = '\n';
            enter_pressed = 1;
        }
        else if (keycode < 128) {
            char c;
            if(is_shift) c = keymap_maj[keycode];
            else c = keymap[keycode];
            putchar(c);
            if(read_buf && read_idx < read_len) read_buf[read_idx++] = c;
        }
    }
    outb(PIC1_COMMAND, 0x20);
}

uint32_t read(FILE *f, uint8_t *buf, uint32_t len) {
    (void)f;
    read_buf = (char *)buf;
    read_len = len;
    read_idx = 0;
    enter_pressed = 0;
    while(!enter_pressed && read_idx < read_len) {
        asm volatile("sti; hlt");
    }
    read_buf = 0;
    return read_idx;
}


uint32_t write(FILE *f, uint8_t *buf, uint32_t len) {
    (void)f;
    uint32_t tmp = len;
    while (tmp--) putchar(*buf++);
    return len;
}

uint32_t error(FILE *f, uint8_t *buf, uint32_t len) {
    (void)f;
    setColor(RED);
    uint32_t tmp = len;
    while (tmp--) putchar(*buf++);
    setColor(WHITE);
    return len;
}

uint32_t k_fread(FILE *f, uint8_t *buf, uint32_t len) {
    return fread(buf, 1, len, f);
}

uint32_t k_fwrite(FILE *f, uint8_t *buf, uint32_t len) {
    return fwrite(buf, 1, len, f);
}

uint32_t k_fclose(FILE *f) {
    return fclose(f);
}

typedef uint32_t (*read_f)(FILE*, uint8_t *, uint32_t);
typedef uint32_t (*write_f)(FILE*, uint8_t *, uint32_t);
typedef uint32_t (*close_f)(FILE*);

struct {
    uint8_t present;
    FILE *f;
    read_f read;
    write_f write;
    close_f close;
} fds[256] = {0};

int find_free_fd() {
    for (int i = 0; i < 256; ++i)
        if (!fds[i].present) return i;
    return -1;
}

int setup_fds() {
    fds[0].present = 1;
    fds[0].f = NULL;
    fds[0].read = read;
    fds[0].write = NULL;
    fds[0].close = NULL;

    fds[1].present = 1;
    fds[1].f = NULL;
    fds[1].read = NULL;
    fds[1].write = write;
    fds[1].close = NULL;

    fds[2].present = 1;
    fds[2].f = NULL;
    fds[2].read = NULL;
    fds[2].write = error;
    fds[2].close = NULL;

    return 0;
}

uint32_t handle_syscall(regs_t *r) {
    asm volatile("sti");
    switch (r->eax) {
        case 3: { // read
            uint32_t fd = r->ebx;
            if (!fds[fd].present) break;
            if (fds[fd].read == NULL) break;
            return fds[fd].read(fds[fd].f, (uint8_t *)r->ecx, r->edx);
        };break;
        case 4: { // write
            uint32_t fd = r->ebx;
            if (!fds[fd].present) break;
            if (fds[fd].write == NULL) break;
            return fds[fd].write(fds[fd].f, (uint8_t *)r->ecx, r->edx);
        };break;
        case 5: { // open
            char *filename = (char *)r->ebx;
            int fd = find_free_fd();
            if (fd == -1) return -1;
            fds[fd].present = 1;
            fds[fd].read = k_fread;
            fds[fd].write = k_fwrite;
            fds[fd].close = k_fclose;
            
            FILE *fp = fopen(filename, "");
            if (fp == NULL) return -1;
            fds[fd].f = fp;
            
            return fd;
        };break;
        case 6: { // close
            uint32_t fd = r->ebx;
            if (fd > 255) return -1;
            if (!fds[fd].present) return -1;
            fds[fd].present = 0;

            fds[fd].close(fds[fd].f);
            fds[fd].f = NULL;
            fds[fd].read = NULL;
            fds[fd].write = NULL;
            fds[fd].close = NULL;
            return 0;
        };break;
        case 0x29: { // dup
            uint32_t fd = r->ebx;
            if (fd > 255) return -1;
            if (!fds[fd].present) return -1;
            int new_fd = find_free_fd();
            if (new_fd == -1) return -1;
            fds[new_fd].present = fds[fd].present;
            fds[new_fd].f = fds[fd].f;
            fds[new_fd].read = fds[fd].read;
            fds[new_fd].write = fds[fd].write;
            return new_fd;
        };break;
        case 0x3f: { // dup2
            uint32_t old_fd = r->ebx;
            if (old_fd > 255) return -1;
            if (!fds[old_fd].present) return -1;
            uint32_t new_fd = r->ecx;
            if (new_fd > 255) return -1;
            if (old_fd == new_fd) return new_fd;

            fds[new_fd].present = fds[old_fd].present;
            fds[new_fd].f = fds[old_fd].f;
            fds[new_fd].read = fds[old_fd].read;
            fds[new_fd].write = fds[old_fd].write;
            return new_fd;
        };break;
        case 0x5a: { // mmap
            return (uint32_t)mmap(r->ebx, r->ecx, r->esi);
        };break;
        case 1: {
            putpixel(r->ebx, r->ecx, r->edx);
        };break;
    }
    return -1;
}
