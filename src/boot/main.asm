[org 0x7c00]

UPPER_MEMORY equ 0x800
EXTENDED_MEMORY equ 0x802
KERNEL_LOCATION equ 0x1000

; Saving disk ID
mov [BOOT_DISK], dl

; Setup stack
xor ax, ax
mov es, ax
mov ds, ax
mov bp, 0x8000
mov sp, bp

; Retrieve memory size
xor cx, cx
xor dx, dx
mov ax, 0xE801
int 0x15
mov word[UPPER_MEMORY], cx
mov word[EXTENDED_MEMORY], dx

; Reading kernel into disk
mov bx, KERNEL_LOCATION
mov dh, 2

mov ah, 0x02
mov al, 0x0f
mov ch, 0x00
mov dh, 0x00
mov cl, 0x02
mov dl, [BOOT_DISK]
int 0x13

; Setup Video Mode (800x600 24bit color)
mov ax, 0x4f02
mov bx, 0x115
int 0x10

; Retrieving VESA config to 0x7e00
mov cx, 0x115
mov ax, 0x0000
mov es, ax
mov di, 0x7e00
mov ax, 0x4F01
int 0x10

; Switch to protected mode
CODE_SEG equ GDT_code - GDT_start
DATA_SEG equ GDT_data - GDT_start

cli
lgdt [GDT_descriptor]
mov eax, cr0
or eax, 1
mov cr0, eax
jmp CODE_SEG:start_protected_mode

jmp $

BOOT_DISK: db 0

GDT_start:
    GDT_null:
        dd 0x0
        dd 0x0

    GDT_code:
        dw 0xffff
        dw 0x0
        db 0x0
        db 0b10011010
        db 0b11001111
        db 0x0

    GDT_data:
        dw 0xffff
        dw 0x0
        db 0x0
        db 0b10010010
        db 0b11001111
        db 0x0

GDT_end:

GDT_descriptor:
    dw GDT_end - GDT_start - 1
    dd GDT_start


[bits 32]
; Protected mode entry
start_protected_mode:
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    ; Setup 32 bit stack base
    mov ebp, 0x90000 ; address can be used by EBDA, be careful to this in case of a bug
    mov esp, ebp

    ; Run kernel
    jmp KERNEL_LOCATION

times 442-($-$$) db 0
