[bits 32]

extern handle_keyboard
extern handle_mouse
extern int_handle

global load_idt
global enable_interrupts
global keyboard_handler
global mouse_handler
global int_handler

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

mouse_handler:
    pushad
    cld
    call handle_mouse
    popad
    iretd


int_handler:
    pushad
    cld
    call int_handle
    popad
    iretd
    ret
