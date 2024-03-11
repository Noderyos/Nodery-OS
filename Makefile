SRC_DIR = src
OBJ_DIR = obj

SRCS = $(shell find $(SRC_DIR) -type f -name "*.c")
OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

ASMS = $(shell find $(SRC_DIR) -name "*.asm" -not -path "$(SRC_DIR)/boot/*")
OBJS += $(patsubst $(SRC_DIR)/%.asm, $(OBJ_DIR)/%_asm.o, $(ASMS))


CC = gcc
CFLAGS = -Wall -Iinclude -ffreestanding -m32 -fno-pie -fno-stack-protector

KERNEL = $(OBJ_DIR)/full_kernel.bin
TARGET = kernel.bin

MKDIR_P = mkdir -p

all: kernel boot
	ld -m elf_i386 -T scripts/linker.ld -o $(KERNEL) -Ttext 0x1000 $(OBJ_DIR)/boot/entry.o $(OBJS) --oformat binary
	cat $(OBJ_DIR)/boot/main.o $(KERNEL) > $(TARGET) 

kernel: $(OBJS)

boot:
	$(MKDIR_P) $(OBJ_DIR)/boot 
	nasm src/boot/main.asm -f bin -o $(OBJ_DIR)/boot/main.o
	nasm src/boot/entry.asm -f elf -o $(OBJ_DIR)/boot/entry.o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@$(MKDIR_P) $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%_asm.o: $(SRC_DIR)/%.asm
	@$(MKDIR_P) $(dir $@) 
	nasm -f elf32 $< -o $@

clean:
	rm -rf $(OBJS) $(TARGET) $(KERNEL) $(OBJ_DIR)

