#include "process.h"

pid32_t next_process(void)
{
    pid32_t selected = SYSERR;
    pri16_t highest_priority = -1;
    pid32_t pid;

    for (pid = 1; pid < NPROC; pid++) {
        if (proctab[pid].prstate == PR_READY &&
            (selected == SYSERR || proctab[pid].prprio > highest_priority)) {
            selected = pid;
            highest_priority = proctab[pid].prprio;
        }
    }

    return selected;
}
