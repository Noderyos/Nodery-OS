CFLAGS=-Wall -Wextra -Iinclude


all: bootloader/boot.asm kernel/entry.asm kernel/kernel.c
	mkdir -p obj
	nasm -f bin bootloader/boot.asm -o obj/boot
	nasm -f elf32 kernel/entry.asm -o obj/entry.o
	nasm -f elf32 kernel/ports.asm -o obj/ports.o
	i386-elf-gcc $(CFLAGS) -c -o obj/kernel.o kernel/kernel.c -nolibc -nostdlib
	i386-elf-gcc $(CFLAGS) -c -o obj/io.o kernel/io.c -nolibc -nostdlib
	i386-elf-gcc $(CFLAGS) -c -o obj/ps2.o kernel/ps2.c -nolibc -nostdlib
	i386-elf-ld -T linker.ld -o obj/kernel.bin obj/entry.o obj/kernel.o obj/io.o obj/ps2.o obj/ports.o --oformat binary
	cat obj/boot obj/kernel.bin > os.bin
	dd if=/dev/zero of=os.bin bs=1k count=64 oflag=append conv=notrunc 
debug: all
	qemu-system-i386 -m 512M -hda os.bin -s -S

run: all
	qemu-system-i386 -m 512M -hda os.bin

clean:
	rm -fr obj os.bin
