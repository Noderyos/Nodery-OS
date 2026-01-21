#include "sys/pic.h"
#include "sys/ps2.h"
#include "sys/ports.h"
#include "io.h"

struct IDT_entry IDT[256];

void isr0(), isr1(), isr2(), isr3(), isr4(), isr5(), isr6(), isr7(),
     isr8(), isr9(), isr10(),isr11(),isr12(),isr13(),isr14(),isr15(),
     isr16(),isr17(),isr18(),isr19(),isr20(),isr21(),isr22(),isr23(),
     isr24(),isr25(),isr26(),isr27(),isr28(),isr29(),isr30(),isr31();

void (*exceptions[32])() = {
    isr0, isr1, isr2, isr3, isr4, isr5, isr6, isr7,
    isr8, isr9, isr10,isr11,isr12,isr13,isr14,isr15,
    isr16,isr17,isr18,isr19,isr20,isr21,isr22,isr23,
    isr24,isr25,isr26,isr27,isr28,isr29,isr30,isr31
};

struct exception_stack {
    uint32_t gs, fs, es, ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags;
};

void cpu_exception_handler(struct exception_stack *r) {
    printf("============ CPU Exception ============\n");
    printf("INT  : %d  ERR: 0x%x\n", r->int_no, r->err_code);
    printf("EIP  : 0x%x  EFLAGS: 0x%x\n", r->eip, r->eflags);
    printf("EAX=0x%x EBX=0x%x ECX=0x%x EDX=0x%x\n", r->eax, r->ebx, r->ecx, r->edx);
    printf("ESI=0x%x EDI=0x%x EBP=0x%x ESP=0x%x\n", r->esi, r->edi, r->ebp, r->esp);
    printf("CS=0x%x DS=0x%x ES=0x%x FS=0x%x GS=0x%x\n", r->cs, r->ds, r->es, r->fs, r->gs);
    printf("========================================\n");

    for(;;) __asm__ volatile("hlt");
}

void init_idt() {
    IDT[0x20].segment = 8;
    IDT[0x20].zero = 0;
    IDT[0x20].type = 0b10001110;
    IDT[0x20].offset_lower = (uint32_t)tick_handler & 0xFFFF;
    IDT[0x20].offset_upper = ((uint32_t)tick_handler & 0xFFFF0000) >> 16;

    IDT[0x21].segment = 8;
    IDT[0x21].zero = 0;
    IDT[0x21].type = 0b10001110;
    IDT[0x21].offset_lower = (uint32_t)keyboard_handler & 0xFFFF;
    IDT[0x21].offset_upper = ((uint32_t)keyboard_handler & 0xFFFF0000) >> 16;

    IDT[0x2C].segment = 8;
    IDT[0x2C].zero = 0;
    IDT[0x2C].type = 0b10001110;
    IDT[0x2C].offset_lower = (uint32_t)mouse_handler & 0xFFFF;
    IDT[0x2C].offset_upper = ((uint32_t)mouse_handler & 0xFFFF0000) >> 16;


    for(int i = 0; i < 32; i++) {
        IDT[i].segment = 8;
        IDT[i].zero = 0;
        IDT[i].type = 0b10001110;
        IDT[i].offset_lower = (uint32_t)exceptions[i] & 0xFFFF;
        IDT[i].offset_upper = ((uint32_t)exceptions[i] & 0xFFFF0000) >> 16;
    }

    IDT[0x69].segment = 8;
    IDT[0x69].zero = 0;
    IDT[0x69].type = 0b10001110;
    IDT[0x69].offset_lower = (uint32_t)syscall_handler & 0xFFFF;
    IDT[0x69].offset_upper = ((uint32_t)syscall_handler & 0xFFFF0000) >> 16;


    // Restart PICs
    outb(PIC1_COMMAND, 0x11); 
    outb(PIC2_COMMAND, 0x11); 

    // Start PIC1 at 0x20 and PIC2 at 0x28
    outb(PIC1_DATA, 0x20);
    outb(PIC2_DATA, 0x28);

    // Configure PIC Cascade mode
    outb(PIC1_DATA, 4);
    outb(PIC2_DATA, 2);

    // Set 8086/88 mode
    outb(PIC1_DATA, 0x1);
    outb(PIC2_DATA, 0x1);

    // Mask all interrupts
    outb(PIC1_DATA, 0xFF);
    outb(PIC2_DATA, 0xFF);

    struct IDT_pointer idt_ptr;
    idt_ptr.offset = (uint32_t)&IDT;
    idt_ptr.limit = sizeof(struct IDT_entry)*256;
    load_idt(&idt_ptr);
}

void init_pit(uint16_t divisor) {
    outb(0x43, 0b00110110);
    outb(0x40, divisor & 0xFF);
    outb(0x40, divisor >> 8);
} 

void kb_init() {
}

void mask_interrupts() {
    outb(PIC1_DATA, 0b11111000); // Unmask IRQ0, IRQ1 & IRQ2
    outb(PIC2_DATA, 0b11101111); // Unmask IRQ4 
}

void mouse_init() {
  
    // Enable second PS/2
    outb(PS2_COMMAND, 0xA8);

    outb(PS2_COMMAND, 0x20);
    uint8_t status = inb(PS2_DATA);
    status |= 0x2;
    outb(PS2_COMMAND, 0x60);
    outb(PS2_DATA, status);

    outb(PS2_COMMAND, 0xD4);
    outb(PS2_DATA, 0xF4);
}
