[bits 32]

global inb
global outb

inb:
    mov edx, [esp + 4]
    in eax, dx
    ret

outb:
    mov edx, [esp + 4]
    mov eax, [esp + 8]
    out dx, al
    ret
