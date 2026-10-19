#include "types.h"
#include "process.h"
#include "memory.h"
#include "string.h"

static pid32_t newpid(void);


pid32_t create(void (*funcaddr), uint32_t ssize, pri16_t priority, char *name, uint32_t nargs, ...){
    uint32_t savsp, *pushsp;
    pid32_t pid;
    struct procent *prptr;
    int32_t i;
    uint32_t *a;
    uint32_t *saddr;

    if (ssize < MINSTK)
        ssize = MINSTK;
    ssize = (uint32_t)highbyte(ssize);

    if ((pid = newpid()) == SYSERR)
    {
        return SYSERR;
    }

    saddr = (uint32_t *)stkcrt(ssize);

    if (saddr == NULL) {
        return SYSERR;
    }

    prcount++;
    prptr = &proctab[pid];
    prptr->prentry = (procfunc_t)funcaddr; //temporary
    prptr->prstate = PR_SUSP;
    prptr->prprio = priority;
    prptr->prstklen = ssize;
    prptr->prstkptr = (char *)saddr;
    prptr->prstkbase = (char *)saddr - ssize + sizeof(uint32_t);
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

    *(uint32_t *)prptr->prstkbase = STACKMAGIC;
    // savsp = (uint32_t)saddr;

    // a = (uint32_t *)(&nargs + 1);
    // a += nargs - 1;

    // for (; nargs > 0; nargs--) *--saddr = *a--;

    // *--saddr = (long)INITRET;
    // *--saddr = (long)funcaddr;
    // *--saddr = savsp;
    // savsp = (uint32_t)saddr;

    // *--saddr = 0x00000200;
    // *--saddr = 0;
    // *--saddr = 0;
    // *--saddr = 0;
    // *--saddr = 0;
    // *--saddr = 0;
    // pushsp = saddr;
    // *--saddr = savsp;
    // *--saddr = 0;
    // *--saddr = 0;

    // *pushsp = (uint32_t)(prptr->prstkptr = (char *)saddr);
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