SRC_DIR = src
OBJ_DIR = obj

SRCS = $(shell find $(SRC_DIR) -type f -name "*.c")
OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

ASMS = $(shell find $(SRC_DIR) -name "*.asm" -not -path "$(SRC_DIR)/boot/*")
OBJS += $(patsubst $(SRC_DIR)/%.asm,$(OBJ_DIR)/%_asm.o, $(ASMS))


CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -ffreestanding -m32 -fno-pie -fno-stack-protector

KERNEL = $(OBJ_DIR)/full_kernel.bin
TARGET = os.img

MKDIR_P = mkdir -p

$(TARGET):
	@echo "Creating disk file"
	@dd if=/dev/zero of=$(TARGET) bs=1024 count=32768 > /dev/null

	@echo "Formating disk file"
	@bash scripts/format.sh $(TARGET)

all: kernel boot $(TARGET)
	@echo "Linking kernel"
	@ld -m elf_i386 -T scripts/linker.ld -o $(KERNEL) $(OBJ_DIR)/boot/entry.o $(OBJS) --oformat binary

	@echo "Writing bootloader"
	@dd conv=notrunc if=obj/boot/main.o of=$(TARGET) > /dev/null

	@echo "Writing kernel"
	@dd conv=notrunc if=obj/full_kernel.bin of=$(TARGET) bs=1 seek=512 > /dev/null

run: all
	qemu-system-x86_64 -m 512M -hda $(TARGET) -s

kernel: $(OBJS)

boot:
	@$(MKDIR_P) $(OBJ_DIR)/boot 

	@echo "NASM bootloader"
	@nasm src/boot/main.asm -f bin -o $(OBJ_DIR)/boot/main.o
	@nasm src/boot/entry.asm -f elf -o $(OBJ_DIR)/boot/entry.o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@$(MKDIR_P) $(dir $@)

	@echo "CC `basename $@`"
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%_asm.o: $(SRC_DIR)/%.asm
	@$(MKDIR_P) $(dir $@) 

	@echo "NASM `basename $@`"
	@nasm -f elf32 $< -o $@

clean:
	rm -rf $(OBJS) $(TARGET) $(KERNEL) $(OBJ_DIR)
