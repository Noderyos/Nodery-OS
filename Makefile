SRC_DIR = src
OBJ_DIR = obj

SRCS = $(shell find $(SRC_DIR) -type f -name "*.c")

OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

BOOTSRCS = $(shell find $(SRC_DIR)/boot -type f -name "*.asm")
BOOTOBJS = $(patsubst $(SRC_DIR)/%.asm,$(OBJ_DIR)/%.o,$(BOOTSRCS))

CC = gcc
CFLAGS = -Wall -Iinclude -ffreestanding -m32 -fno-pie

KERNEL = $(OBJ_DIR)/full_kernel.bin
TARGET = kernel.bin

MKDIR_P = mkdir -p

all: kernel boot
	bin/i386-elf-ld -o $(KERNEL) -Ttext 0x1000 $(OBJ_DIR)/boot/entry.o $(OBJS) $(OBJ_DIR)/kernel/asm_keyboard.o --oformat binary
	cat $(OBJ_DIR)/boot/main.o $(KERNEL) > $(TARGET) 

kernel: $(OBJS)
	nasm -f elf32 -o $(OBJ_DIR)/kernel/asm_keyboard.o src/kernel/keyboard.asm

boot: 
	$(MKDIR_P) $(OBJ_DIR)/boot 
	nasm src/boot/main.asm -f bin -o $(OBJ_DIR)/boot/main.o
	nasm src/boot/entry.asm -f elf -o $(OBJ_DIR)/boot/entry.o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@$(MKDIR_P) $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	rm -rf $(OBJS) $(TARGET) $(KERNEL) $(OBJ_DIR)
