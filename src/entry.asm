bits 32
section .text

extern main

call main

%include "src/kernel/keyboard.asm"

jmp $