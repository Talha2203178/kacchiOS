#include "kacchi.h"

void xdone(void)
{
    serial_puts("Process is done\n");
    while (1)
        ; 
}