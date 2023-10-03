bits 32
section .text

extern main

call main ; Call the main function in kernel.c

jmp $ ; Do nothing