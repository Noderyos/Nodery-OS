[bits 32]
section .text

global test_syscall

extern main

call main
jmp $

test_syscall:
    ret

filename: db "/HELLO.TXT", 0
hello: resb 16
