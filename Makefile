AS=nasm
CC=i386-elf-gcc
LD=i386-elf-ld

CFLAGS=-Wall -Wextra -Wno-packed-bitfield-compat -Iinclude -nolibc -nostdlib -ffreestanding -fpack-struct
LDFLAGS=-T linker.ld --oformat binary -Map=layout.map

OBJDIR=obj
C_SRCS= \
	kernel/kernel.c \
	kernel/io.c \
	kernel/string.c \
	kernel/fat.c \
	kernel/ui.c \
	kernel/syscalls.c \
	kernel/mmap.c \
	kernel/malloc.c \
	kernel/elf.c \
	kernel/scheduler.c \
	kernel/sys/longlong.c \
	kernel/sys/lba.c \
	kernel/sys/ps2.c \
	kernel/sys/vga.c \
	kernel/sys/timer.c \
	kernel/sys/io.c

ASM_SRCS= \
    kernel/entry.asm \
    kernel/sys/pic.asm \
    kernel/sys/ports.asm

C_OBJS=$(patsubst %.c,$(OBJDIR)/%.o,$(C_SRCS))
ASM_OBJS=$(patsubst %.asm,$(OBJDIR)/%.o,$(ASM_SRCS))
OBJS=$(ASM_OBJS) $(C_OBJS)


all: os.bin 

os.bin: obj/boot obj/kernel.bin part.bin
	@echo "OUT   $@"
	@dd if=/dev/zero      of=os.bin bs=512 count=2048
	@dd if=obj/boot       of=os.bin bs=512           conv=notrunc
	@dd if=obj/kernel.bin of=os.bin bs=512 seek=1    conv=notrunc
	@dd if=part.bin       of=os.bin bs=512 seek=2048 conv=notrunc

obj/kernel.bin: $(OBJS)
	@echo "LD    $@"
	@$(LD) $(LDFLAGS) -o $@ $^


$(OBJDIR)/boot: bootloader/boot.asm
	@mkdir -p $(dir $@)
	@echo "BOOT  $<"
	@$(AS) -f bin $< -o $@

$(OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@echo "CC    $<"
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o: %.asm
	@mkdir -p $(dir $@)
	@echo "AS    $<"
	@$(AS) -f elf32 $< -o $@


debug: os.bin
	qemu-system-i386 -m 512M -hda $< -s -S

run: os.bin
	qemu-system-i386 -m 512M -hda $<

clean:
	rm -fr obj os.bin layout.map
