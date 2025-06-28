[bits 16]
[org 0x7c00]

BOOT_DISK equ 0x800
KERNEL_LOCATION equ 0x1000

UPPER_MEM equ 0x802
EXTENDED_MEM equ 0x804
VBE equ 0x1000


mov [BOOT_DISK], dl

mov ax, 0xE801
int 15h
mov word[UPPER_MEM], cx
mov word[EXTENDED_MEM], dx

; Setup stack
mov sp, 0x4000
mov bp, 0x4000

; Set video mode (800 x 600, 16m colors)
mov ax, 0x4f02
mov bx, 0x115
int 10h

; Retrieve VBE informations
mov ax, 0
mov es, ax
mov ax, 0x4F01
mov cx, 0x0115
mov di, 0x7e00
int 10h

; Read disk
mov ah, 0x2
mov al, 0x20 ; Sector count
mov ch, 0x0
mov dh, 0x0
mov cl, 0x2
mov bx, KERNEL_LOCATION ; Address
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

%include "bootloader/print.asm"

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

    jmp KERNEL_LOCATION

jmp $

times 446-($-$$) db 0

mbr_part_1:
    db 0x00
    db 0xff, 0xff, 0xff
    db 0x83
    db 0xff, 0xff, 0xff
    dd 2048
    dd 32768
mbr_part_2:
    dd 0, 0, 0, 0
mbr_part_3:
    dd 0, 0, 0, 0
mbr_part_4:
    dd 0, 0, 0, 0






dw 0xaa55
