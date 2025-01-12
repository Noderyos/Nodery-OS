
all: boot.asm
	nasm -f bin boot.asm -o boot

run: all
	qemu-system-i386 -m 512M -hda boot
