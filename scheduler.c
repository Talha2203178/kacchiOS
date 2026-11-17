#include "types.h"
#include "process.h"
#include "io.h"
#include "memory.h"
#include "queue.h"
#include "serial.h"
#include "string.h"
#include "semaphore.h"
#include <interrupts.h>
syscall resched(void)
{
    struct procent *oldproc;
    struct procent *newproc;
    pid32_t nextpid;

    uint32_t mask = irq_save();

    oldproc = &proctab[currpid];
    if (oldproc->prstate == PR_CURR) {
       if (nonempty(readylist) &&
            oldproc->prprio > firstkey(readylist)) {
            //serial_puts("\nresched: putting current process back to ready list\n");
            //irq_restore(mask);
            return OK;
        } 
        oldproc->prstate = PR_READY;
        if (insert(currpid, readylist, oldproc->prprio) == SYSERR) {
            oldproc->prstate = PR_CURR;
            irq_restore(mask);
            serial_puts("Here1\n");
            return SYSERR;
        }
    }

    nextpid = dequeue(readylist);
    if (nextpid == EMPTY || nextpid == SYSERR) {
        oldproc->prstate = PR_CURR;
        irq_restore(mask);
        serial_puts("Here2\n");
        return SYSERR;
    }

    currpid = nextpid;
    newproc = &proctab[currpid];
    newproc->prstate = PR_CURR;
    //serial_puts("Before ctxsw\n");
    ctxsw(&oldproc->prstkptr, newproc->prstkptr);
    irq_restore(mask);
    return OK;
}

int ready(pid32_t pid)
{
    register struct procent *prptr;

    uint32_t mask = irq_save();
    if (isbadpid(pid)) {
        irq_restore(mask);
        return SYSERR;
    }

    prptr = &proctab[pid];
    prptr->prstate = PR_READY;
    insert(pid, readylist, prptr->prprio);
    resched();

    irq_restore(mask);
    return OK;
}