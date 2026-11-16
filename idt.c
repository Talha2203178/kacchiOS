#include <idt.h>
#include <process.h>
#include <io.h>
#include <serial.h>

static void idt_gate(uint8_t idt_num, uint16_t segment, uint32_t offset, uint8_t types);
void init_idt();

struct idt_list idt[256];
struct idt_pos idtp;

static void idt_gate(uint8_t idt_num, uint16_t segment, uint32_t offset, uint8_t types){
    idt[idt_num].offset_low = offset & 0xFFFF;
    idt[idt_num].offset_high = (offset >> 16) & 0xFFFF;
    idt[idt_num].segmentaddress = segment;
    idt[idt_num].reserved = 0;
    idt[idt_num].type_attributes = types;
}

void init_idt(){
    idtp.limit = (sizeof(struct idt_list) * 256) - 1;
    idtp.base = (uint32_t)&idt;

    //memset(&idt, 0, sizeof(struct idt_list) * 256);
    for (int i = 0; i < 256; i++) {
        idt[i].offset_low = 0;
        idt[i].offset_high = 0;
        idt[i].segmentaddress = 0;
        idt[i].type_attributes = 0;
        idt[i].reserved = 0;
    }
    idt_gate(32, 0x08, (uint32_t)timer_interrupt_handler, 0x8E);

    outb(0x20, 0x13);
    outb(0x21, 0x20);
    outb(0x21, 0x01);

    outb(0x43, 0x36);
    outb(0x40, 0x9B);
    outb(0x40, 0x2E);
    
    set_idt_reg((uint32_t)&idtp);
}

void timer_interrupt_2(void){
    serial_puts("Test Interrupt IN the end it doesn't even matter\n");
    outb(0x20, 0x20);
    resched();
}
//https://wiki.osdev.org/Interrupt_Descriptor_Table
//https://wiki.osdev.org/Interrupt_Service_Routines
//https://www.brokenthorn.com/Resources/OSDevIndex.html
//https://wiki.osdev.org/8259_PIC
//https://github.com/Exclavia/Kernel-Dev/blob/main/chapters/05-irq-and-pit.md
//https://www.brokenthorn.com/Resources/OSDevPic.html
//https://www.brokenthorn.com/Resources/OSDevPit.html
//https://www.intel.com/content/www/us/en/content-details/782158/intel-64-and-ia-32-architectures-software-developer-s-manual-combined-volumes-1-2a-2b-2c-2d-3a-3b-3c-3d-and-4.html?wapkw=intel%2064%20and%20ia-32%20architectures%20software%20developer%27s%20manual&docid=782161