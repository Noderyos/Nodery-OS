
all: boot.asm
	nasm -f bin boot.asm -o boot

debug: all
	qemu-system-i386 -m 512M -hda boot -s -S

run: all
	qemu-system-i386 -m 512M -hda boot
