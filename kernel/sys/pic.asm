[bits 32]

extern handle_keyboard
extern handle_mouse
extern int_handle
extern handle_tick
extern cpu_exception_handler
extern handle_syscall

global load_idt
global enable_interrupts
global keyboard_handler
global mouse_handler
global tick_handler
global int_handler
global syscall_handler

extern scheduler
extern current_task

load_idt:
    mov edx, [esp + 4]
    lidt [edx]
    ret

enable_interrupts:
    sti
    ret

tick_handler:
    pusha
    mov eax, [current_task]
    mov [eax], esp

    call handle_tick
    call scheduler

    mov eax, [current_task]
    mov esp, [eax]

    mov ecx, [eax+4]
    mov cr3, ecx

    popa
    iretd

   

;tick_handler:
;    pushad
;    cld
;    call handle_tick
;    popad
;    iretd


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

ret_val: dd 0

syscall_handler:
    pushad
    cld
    push esp
    call handle_syscall
    mov dword[ret_val], eax
    pop eax
    popad
    mov eax, dword[ret_val]
    iretd

%macro ISR_NOERR 2
global isr%1
isr%1:
    push 0
    push %1
    jmp isr_common_stub
%endmacro

%macro ISR_ERR 2
global isr%1
isr%1:
    push %1
    jmp isr_common_stub
%endmacro

isr_common_stub:
    pusha
    push ds
    push es
    push fs
    push gs
    push esp   ; ptr to the stack => struct with all values previously pushed
    call cpu_exception_handler
    ; never returns

ISR_NOERR 0,  divide_by_zero
ISR_NOERR 1,  debug
ISR_NOERR 2,  nmi
ISR_NOERR 3,  breakpoint
ISR_NOERR 4,  overflow
ISR_NOERR 5,  bound
ISR_NOERR 6,  invalid_opcode
ISR_NOERR 7,  device_na
ISR_NOERR 9,  coprocessor_segment_overrun
ISR_NOERR 15, reserved
ISR_NOERR 16, fpu_error
ISR_NOERR 18, machine_check
ISR_NOERR 19, simd
ISR_NOERR 20, virt
ISR_NOERR 22, reserved
ISR_NOERR 23, reserved
ISR_NOERR 24, reserved
ISR_NOERR 25, reserved
ISR_NOERR 26, reserved
ISR_NOERR 27, reserved
ISR_NOERR 28, hypervisor_injection
ISR_NOERR 31, reserved

ISR_ERR 8,  double_fault
ISR_ERR 10, invalid_tss
ISR_ERR 11, segment_not_present
ISR_ERR 12, stack_segment_fault
ISR_ERR 13, general_protection
ISR_ERR 14, page_fault
ISR_ERR 17, alignment_check
ISR_ERR 21, control_protection
ISR_ERR 29, vmm_communication
ISR_ERR 30, security
