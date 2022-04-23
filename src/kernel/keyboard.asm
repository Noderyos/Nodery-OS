bits 32
section .text

global load_idt
global keyboard_handler
global ioport_in
global ioport_out
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