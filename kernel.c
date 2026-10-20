/* kernel.c - Main kernel with null process */
#include "types.h"
#include "serial.h"
#include "string.h"
#include "memory.h"
#include "process.h"

#define MAX_INPUT 128
// extern struct blockstr freemem; 
//extern void *memsec;


static void test_process(void)
{
    int32_t i;

    for (i = 0; i < 5; i++) {
        serial_puts("process 1 is running\n");
    }

    serial_puts("process 1 is returning\n");
}

void kmain(void) {
    serial_init();
    
    char input[MAX_INPUT];
    int pos = 0;
    struct blockstr *pmblock;  
    // void *testblock;
    // char *input2;

    /* Initialize free memory list */

    char *memory_start;
    memory_start = (char *)highbyte((uint32_t)memsec);

    freemem.next = (struct blockstr *)memory_start;
    freemem.size = 4096;

    pmblock = freemem.next;
    pmblock->size = 4096;
    pmblock->next = NULL;
    // memsec = highbyte(memsec);
    // freemem.next = pmblock = (struct blockstr *)memsec;
    // freemem.size = (uint32_t)(4026);
    // pmblock->size = (uint32_t)(4026);
    // pmblock->next = NULL;
    // input2 = (char *)freemem.size;
    // serial_puts("\nHere\n");
    // serial_puts(input2);
    // serial_puts("\n");
    // heapcrt(22);
    // testblock = heapcrt(22);
    // input2 = (char *)heapkill((void *)testblock, 22);
    // serial_puts(input2);
    // serial_puts("\n");
    // stkcrt(22);
    // testblock = stkcrt(22);
    // input2 = (char *)stkkill((void *)testblock, 22);
    // serial_puts(input2);
    
    /* Initialize hardware */

    pid32_t pid;
    pid32_t second_pid;
    pid32_t third_pid;

    proc_init();

    serial_puts("creating process\n");

    pid = create(test_process, 1024, 10, "test", 0);

    if (pid == SYSERR) {
        serial_puts("create failed\n");
        return;
    }

    serial_puts("process created\n");

    if (resume(pid) == SYSERR) {
        serial_puts("resume failed\n");
        return;
    }

    second_pid = create(test_process, 1024, 30, "high", 0);
    third_pid = create(test_process, 1024, 20, "medium", 0);

    if (second_pid == SYSERR || third_pid == SYSERR ||
        resume(second_pid) == SYSERR || resume(third_pid) == SYSERR) {
        serial_puts("additional process setup failed\n");
        return;
    }

    serial_puts("process is ready\n");

    while (resched() != SYSERR) {
        serial_puts("scheduler selected process\n");
    }

    serial_puts("process terminated\n");
    
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