[bits 32]

extern handle_keyboard

global inb
global outb
global load_idt
global enable_interrupts
global keyboard_handler


load_idt:
    mov edx, [esp + 4]
    lidt [edx]
    ret

enable_interrupts:
    sti
    ret

keyboard_handler:
    pushad
    cld
    call handle_keyboard
    popad
    iretd

inb:
    mov edx, [esp + 4]
    in eax, dx
    ret

outb:
    mov edx, [esp + 4]
    mov eax, [esp + 8]
    out dx, al
    ret
