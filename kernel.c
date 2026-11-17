/* kernel.c - Main kernel with null process */
#include "types.h"
#include "serial.h"
#include "string.h"
#include "memory.h"
#include "process.h"
#include "semaphore.h"
#include <ipc.h>

#define MAX_INPUT 128
// extern struct blockstr freemem; 
//extern void *memsec;

sid32_t test_sem;

void test_time_quanta1(void){
    for(int i = 0; i < 1000; i++){
        serial_puts("Test Process 1\n");
    }
}

void test_time_quanta2(void){
    for(int i = 0; i < 1000; i++){
        serial_puts("Test Process 2\n");
    }
}

void test_recvmsg(void){
    struct msgstk stk;
    stk = rcvmsg();
    serial_puts("Received message:\n");
    serial_puts((char *)stk.stkbase);
    stkkill((void *)stk.stkbase, stk.nobytes);
}
void test_sendmsg(void){
    char *msg = "This is a message\n";
    sendmsg(msg, strlen(msg));
}
static void add_process(char a, char b, char c){
    b = b+1;
    a = a+1;
    serial_puts("add process\n");
    serial_putc(a);
    serial_puts("\n");
    serial_putc(b);
    serial_puts("\n");
    serial_putc(c);
    serial_puts("\n");
    /*char x = '5';
    if (x > '3'){
        x--;
        add_process('a', 'b', 'c');
    }
    else{
        return;
    }*/
}

static void print_process(void){
    serial_puts("Hello\n");
}

static void print_process4(void){
    serial_puts("Hello\n");
}

static void print_process2(void){
    pid32_t  pid = create(print_process4, 1024, 10, "test", 0);
    resume(pid);
    wait(test_sem);
    serial_puts("Hello1\n");
}

static void print_process3(void){
    signal(test_sem);
    serial_puts("Hello2\n");
}

static void test_process(void)
{
    int32_t i;
    test_sem = semcreate(0);
    pid32_t pid;
    for (i = 0; i < 2; i++) {
        pid = create(print_process, 1024, 8, "test", 0);
        resume(pid);
    }
    
    pid = create(print_process2, 1024, 10, "test", 0);
    resume(pid);
    pid = create(print_process3, 1024, 10, "test", 0);
    resume(pid); 
    pid = create(add_process, 1024, 10, "test", 3, 'c', 'b', 'a');
    resume(pid);

     pid = create(print_process4, 1024, 12, "test", 0);
       resume(pid);
    serial_puts("process 1 is returning\n");
    pid = create(test_recvmsg, 1024, 10, "test", 0);
    resume(pid);
    pid = create(test_sendmsg, 1024, 10, "test", 0);
    resume(pid);
    pid = create(test_time_quanta1, 2048, 10, "test", 0);
    //__asm__ volatile("sti");
    resume(pid);
    pid = create(test_time_quanta2, 2048, 10, "test", 0);
    resume(pid);
}

void kmain(void) {
    serial_init();
    
    char input[MAX_INPUT];
    int pos = 0;
    struct blockstr *pmblock;  
    //serial_puts("creating process\n");

    char *memory_start;
    memory_start = (char *)highbyte((uint32_t)memsec);

    freemem.next = (struct blockstr *)memory_start;
    freemem.size = 65536;

    pmblock = freemem.next;
    pmblock->size = 65536;
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
     //serial_puts("creating process2\n");
    pid = create(test_process, 2048, 15, "test", 0);
    // serial_puts("creating process3\n");

    if (pid == SYSERR) {
        serial_puts("create failed\n");
        return;
    }

    //serial_puts("process created\n");

    if (resume(pid) == SYSERR) {
        serial_puts("resume failed\n");
        return;
    }
    serial_puts("process resumed\n");
    
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