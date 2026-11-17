#include "types.h"
#include "process.h"
#include "io.h"
#include "memory.h"
#include "queue.h"
#include "serial.h"
#include "string.h"
#include "semaphore.h"
#include <ipc.h>
#include <idt.h>
#include <interrupts.h>

static pid32_t newpid(void);

pid32_t create(void (*funcaddr), uint32_t ssize, pri16_t priority, char *name, uint32_t nargs, ...){
    pid32_t pid;
    struct procent *prptr;
    int32_t i;
    uint32_t *saddr;
    volatile uint32_t *frame;
    uint32_t *a; 

    uint32_t mask = irq_save();

    if (ssize < MINSTK)
        ssize = MINSTK;
    ssize = (uint32_t)highbyte(ssize);

    if ((pid = newpid()) == SYSERR)
    {
        //irq_restore(mask);
        return SYSERR;
    }

    saddr = (uint32_t *)stkcrt(ssize);

    if (saddr == NULL) {
        //irq_restore(mask);
        return SYSERR;
    }

    prcount++;
    prptr = &proctab[pid];
    prptr->prentry = (procfunc_t)funcaddr; //temporary
    prptr->prstate = PR_SUSP;
    prptr->prprio = priority;
    prptr->prstklen = ssize;
    prptr->prstkptr = (char *)saddr;
    //prptr->prstkbase = (char *)saddr - ssize + sizeof(uint32_t);
    prptr->prstkbase = (char *)saddr;;
    strcpy(prptr->prname, name);
    prptr->prname[PNMLEN - 1] = '\0';
    prptr->prsem = -1;
    prptr->prparent = (pid32_t)currpid;
    prptr->prhasmsg = FALSE;
    for (i = 0; i < NDESC; i++) {
        prptr->prdesc[i] = DEV_NONE;
    }

    prptr->prdesc[0] = DEV_CONSOLE;
    prptr->prdesc[1] = DEV_CONSOLE;
    prptr->prdesc[2] = DEV_CONSOLE;

    a = (uint32_t *)(&nargs + 1);
    a += nargs - 1;

    for (; nargs > 0; nargs--){
        *--saddr = *a--;   //compiler expects initret
    }

    prptr->prstkptr = (char *)saddr;

    /* popal restores eight words, then ret enters the process. */
    frame = (volatile uint32_t *)((char *)prptr->prstkptr -
                                  (11 * sizeof(uint32_t)));
    frame[0] = 0;
    frame[1] = 0;
    frame[2] = 0;
    frame[3] = 0;
    frame[4] = 0;
    frame[5] = 0;
    frame[6] = 0;
    frame[7] = 0;
    frame[8] = 0x202;
    frame[9] = (uint32_t)funcaddr;
    frame[10] = (uint32_t)userret;

    prptr->prstkptr = (char *)frame;
    //serial_puts("process created\n");
    irq_restore(mask);
    //serial_puts("process created2\n");
    return pid;
}

static pid32_t newpid(void)
{
    uint32_t i;
    static pid32_t nextpid = 1;

    for (i = 0; i < NPROC; i++)
    {
        nextpid %= NPROC;
        if (proctab[nextpid].prstate == PR_FREE)
        {
            return nextpid++;
        }
        else
        {
            nextpid++;
        }
    }
    return (pid32_t)SYSERR;
}

int32_t kill(pid32_t pid)
{
    //serial_puts("killing process\n");
    struct procent *prptr;

    uint32_t mask = irq_save();

    if (isbadpid(pid) || pid == NULLPROC) {
        irq_restore(mask);
        return SYSERR;
    }

    prptr = &proctab[pid];

    if(--prcount <= 1) {
        xdone();
    }

    stkkill(prptr->prstkbase, prptr->prstklen);
    //serial_puts("stack killed\n");

    switch (prptr->prstate) {
        case PR_CURR:
            prptr->prstate = PR_FREE;
            //serial_puts("current process killed\n");
            resched();
            break;

        case PR_READY:
            getitem(pid);
            prptr->prstate = PR_FREE;
            break;

        case PR_SUSP:
            prptr->prstate = PR_FREE;
            break;

       case PR_WAIT:
            semtab[prptr->prsem].semcount++;
            break;

        default:
            prptr->prstate = PR_FREE;
    }

    if (currpid == pid) {
        currpid = NULLPROC;
    }
    //serial_puts("process killed\n");
    irq_restore(mask);
    return OK;
}

syscall suspend(pid32_t pid)
{
    struct procent *prptr;

    uint32_t mask = irq_save();

    if (isbadpid(pid) || (pid == NULLPROC))
    {
        irq_restore(mask);
        return SYSERR;
    }

    prptr = &proctab[pid];

    if (prptr->prstate == PR_CURR)
    {
        prptr->prstate = PR_SUSP;
        resched();
    }
    else if (prptr->prstate == PR_READY)
    {
        getitem(pid);
        prptr->prstate = PR_SUSP;
    }
    else
    {
        irq_restore(mask);
        return SYSERR;
    }
    irq_restore(mask);
    return OK;
}

pri16_t resume(pid32_t pid)
{
    struct procent *prptr;

    uint32_t mask = irq_save();

    if (isbadpid(pid)) {
        irq_restore(mask);
        return SYSERR;
    }
    prptr = &proctab[pid];
    if (prptr->prstate != PR_SUSP) {
        irq_restore(mask);
        return SYSERR;
    }
    irq_restore(mask);
    return ready(pid);
}

pid32_t getpid(void)
{
    return currpid;
}

syscall getprio(pid32_t pid)
{
    struct procent *prptr;

    if (isbadpid(pid))
    {
        return SYSERR;
    }

    prptr = &proctab[pid];

    return prptr->prprio;
}

pri16_t chprio(pid32_t pid, pri16_t newprio)
{
    struct procent *prptr;

    if (isbadpid(pid))
    {
        return SYSERR;
    }

    prptr = &proctab[pid];
    pri16_t oldprio = prptr->prprio;
    prptr->prprio = newprio;

    return oldprio;
}

