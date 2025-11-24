section .text

global _start

_start:
    mov eax, 4
    mov ebx, 1
    mov ecx, hello
    mov edx, 14
    int 0x69
    ret

section .data

hello: db "Hello, World!", 10, 0
