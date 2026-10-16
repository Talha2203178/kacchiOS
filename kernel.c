/* kernel.c - Main kernel with null process */
#include "types.h"
#include "serial.h"
#include "string.h"
#include "memory.h"

#define MAX_INPUT 128
//extern struct blockstr freemem; 
//extern void *memsec;

void kmain(void) {
    char input[MAX_INPUT];
    int pos = 0;
    struct blockstr *pmblock;  
    void *testblock;
    char *input2;

    /* Initialize free memory list */
    memsec = highbyte(memsec);
    freemem.next = pmblock = (struct blockstr *)memsec;
    freemem.size = (uint32_t)(4026);
    pmblock->size = (uint32_t)(4026);
    pmblock->next = NULL;
    input2 = (char *)freemem.size;
    serial_puts("\nHere\n");
    serial_puts(input2);
    serial_puts("\n");
    heapcrt(22);
    testblock = heapcrt(22);
    input2 = (char *)heapkill((void *)testblock, 22);
    serial_puts(input2);
    serial_puts("\n");
    stkcrt(22);
    testblock = stkcrt(22);
    input2 = (char *)stkkill((void *)testblock, 22);
    serial_puts(input2);
    
    /* Initialize hardware */
    serial_init();
    
    /* Print welcome message */
    serial_puts("\n");
    serial_puts("========================================\n");
    serial_puts("    kacchiOS - Minimal Baremetal OS\n");
    serial_puts("========================================\n");
    serial_puts("Hello from kacchiOS!\n");
    serial_puts("Running null process...\n\n");
    
    /* Main loop - the "null process" */
    while (1) {
        serial_puts("kacchiOS> ");
        pos = 0;
        
        /* Read input line */
        while (1) {
            char c = serial_getc();
            
            /* Handle Enter key */
            if (c == '\r' || c == '\n') {
                input[pos] = '\0';
                serial_puts("\n");
                break;
            }
            /* Handle Backspace */
            else if ((c == '\b' || c == 0x7F) && pos > 0) {
                pos--;
                serial_puts("\b \b");  /* Erase character on screen */
            }
            /* Handle normal characters */
            else if (c >= 32 && c < 127 && pos < MAX_INPUT - 1) {
                input[pos++] = c;
                serial_putc(c);  /* Echo character */
            }
        }
        
        /* Echo back the input */
        if (pos > 0) {
            //if(strcmp(input, "exit") == 0) 
            serial_puts("You typed: ");
            serial_puts(input);
            serial_puts("\n");
        }
    }
    
    /* Should never reach here */
    for (;;) {
        __asm__ volatile ("hlt");
    }
}