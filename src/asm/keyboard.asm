bits 32

global load_gdt
global load_idt
global keyboard_handler
global ioport_in
global ioport_out
global inl
global outl
global enable_interrupts

extern handle_keyboard_interrupt

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
    call handle_keyboard_interrupt
    popad
    iretd

ioport_in:
    mov edx, [esp + 4]
    in al, dx
    ret

ioport_out:
    mov edx, [esp + 4]
    mov eax, [esp + 8]
    out dx, al
    ret

inl:
    mov edx, [esp + 4]
    in eax, dx
    ret

outl:
    mov edx, [esp + 4]
    mov eax, [esp + 8]
    out dx, eax
    ret
