[bits 16]
[org 0x7c00]

mov sp, 0x4000
mov bp, 0x4000

mov ah, 0
mov al, 0x10
int 10h

mov si, hello
call error

jmp $

error:
    push si
    mov bl, 0xc
    mov di, err

.err_print:
    mov al, [di]
    inc di
 
    cmp al, 0
    je .err_msg_print

    mov ah, 0xe
    mov bh, 0
    int 10h
    jmp .err_print

.err_msg_print:
    mov al, [si]
    inc si
 
    cmp al, 0
    je .err_end

    mov ah, 0xe
    mov bh, 0
    int 10h
    jmp .err_msg_print
.err_end:
    pop si
    ret


jmp $

err: db "ERROR: ", 0
hello: db "Not really an error btw :)", 13, 10, 0

times 510-($-$$) db 0
dw 0xaa55
