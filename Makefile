
all: bootloader/boot.asm kernel/entry.asm kernel/kernel.c
	mkdir -p obj
	nasm -f bin bootloader/boot.asm -o obj/boot
	nasm -f elf32 kernel/entry.asm -o obj/entry.o
	i386-elf-gcc -o obj/kernel.o kernel/kernel.c -nolibc -nostdlib
	i386-elf-ld -T linker.ld -o obj/kernel.bin obj/entry.o obj/kernel.o  --oformat binary
	cat obj/boot obj/kernel.bin > os.bin
debug: all
	qemu-system-i386 -m 512M -hda os.bin -s -S

run: all
	qemu-system-i386 -m 512M -hda os.bin

clean:
	rm -fr obj os.bin
