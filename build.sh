#!/bin/bash

echo "Compiling OS ..."



echo "[nasm] Compiling main.asm"

nasm "src/main.asm" -f bin -o "build/main.bin"



echo "[nasm] Compiling entry.asm"

nasm "src/entry.asm" -f elf -o "build/entry.o"


o_files=""

for c_file in $(find src -name "*.c"); do
	c_name=$(basename $c_file)
	o_file="build/$(echo $c_name | cut -d "." -f1).o"
	echo "[gcc] Compiling $c_name to $o_file"
	gcc -ffreestanding -m32 -g -c "$c_file" -o "$o_file" -fno-pie -ggdb
	o_files="${o_files} $o_file"
done

echo "[nasm] Compiling zeroes.asm"

nasm "src/zeroes.asm" -f bin -o "build/zeroes.bin"

echo $o_files

echo "[ld] > Linking $o_files => full_kernel.bin"

bin/i386-elf-ld -o "build/full_kernel.bin" -Ttext 0x1000 "build/entry.o" $o_files --oformat binary

echo "[cat] > Concatenaing full_kernel.bin + zeroes.bin => OS.bin"

cat "build/main.bin" "build/full_kernel.bin" "build/zeroes.bin"  > "OS.bin"



echo "Starting QEMU"

remmina -c vnc://127.0.0.1:5900 >/dev/null 2>/dev/null &

~/Téléchargements/qemu/build/qemu-system-x86_64 OS.bin >/dev/null 2>/dev/null

