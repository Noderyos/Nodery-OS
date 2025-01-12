[bits 16]
[org 0x7c00]

mov si, hello
print:
    mov al, [si]
    inc si
 
    cmp al, 0
    je end

    mov ah, 0xe
    mov bh, 0
    mov bl, 0xf
    int 10h
    
    jmp print
end:

jmp $

hello: db "Hello from NoderyOS", 0

times 510-($-$$) db 0
dw 0xaa55
