#include <types.h>

struct idt_list{
    uint16_t offset_low;
    uint16_t segmentaddress;
    uint8_t reserved;
    uint8_t type_attributes;
    uint16_t offset_high;
} __attribute__((packed));

struct idt_pos{
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

extern void timer_interrupt_handler(void);
extern void set_idt_reg(uint32_t address);
void init_idt();