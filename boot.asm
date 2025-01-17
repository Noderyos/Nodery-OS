[bits 16]
[org 0x7c00]

BOOT_DISK equ 0x800

mov [BOOT_DISK], dl

; Setup stack
mov sp, 0x4000
mov bp, 0x4000

; Set video mode (640 x 350, 16 colors)
mov ah, 0
mov al, 0x3
int 10h

; Read disk
mov ah, 0x2
mov al, 0x1 ; Sector count
mov ch, 0x0
mov dh, 0x0
mov cl, 0x2
mov bx, 0x7e00 ; Address
mov dl, [BOOT_DISK]
int 13h

; Check if read error
jb read_error

CODE_SEG equ GDT_code - GDT_start
DATA_SEG equ GDT_data - GDT_start

cli
lgdt [GDT_descriptor]
mov eax, cr0
or eax, 1
mov cr0, eax

jmp CODE_SEG:start_protected_mode

read_error:
    mov si, disk_error
    call error
    jmp $

%include "print.asm"

disk_error: db "Cannot read disk", 13, 10, 0

GDT_start:
    GDT_null:
        dd 0
        dd 0
    GDT_code:
        dw 0xFFFF
        dw 0x0
        db 0x0
        db 0b10011011
        db 0b11001111
        db 0x0
    GDT_data:
        dw 0xFFFF
        dw 0
        db 0
        db 0b10010011
        db 0b11001111
        db 0
GDT_end:

GDT_descriptor:
    dw GDT_end - GDT_start - 1
    dd GDT_start

[bits 32]
start_protected_mode:
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov ebp, 0x90000
    mov esp, ebp

    mov byte[0xb8000], 'N'
    mov byte[0xb8001], 0x6

    jmp $

times 510-($-$$) db 0
dw 0xaa55
times 512 db 0
