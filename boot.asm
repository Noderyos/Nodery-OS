[bits 16]
[org 0x7c00]

BOOT_DISK equ 0x800

mov [BOOT_DISK], dl

; Setup stack
mov sp, 0x4000
mov bp, 0x4000

; Set video mode (640 x 350, 16 colors)
mov ah, 0
mov al, 0x10
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
jnb 0x7e00

mov si, disk_error
call error
jmp $

%include "print.asm"

disk_error: db "Cannot read disk", 13, 10, 0

times 510-($-$$) db 0
dw 0xaa55

mov si, hello
call error

jmp $

hello: db "Not a real error BTW :)", 13, 10, 0

times 1000 db 0
