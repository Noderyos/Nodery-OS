[bits 32]

global inb
global outb
global inw
global outw

inb:
    mov edx, [esp + 4]
    xor eax, eax
    in al, dx
    ret

outb:
    mov edx, [esp + 4]
    mov eax, [esp + 8]
    out dx, al
    ret

inw:
    mov edx, [esp + 4]
    xor eax, eax
    in ax, dx
    ret

outw:
    mov edx, [esp + 4]
    mov eax, [esp + 8]
    out dx, ax
    ret
