#include "types.h"
#include "process.h"
#include "memory.h"
#include "string.h"

static pid32_t newpid(void);


pid32_t create(void (*funcaddr), uint32_t ssize, pri16_t priority, char *name, uint32_t nargs, ...){
    pid32_t pid;
    struct procent *prptr;
    int32_t i;
    uint32_t *saddr;
    volatile uint32_t *frame;

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

    /* popal restores eight words, then ret enters the process. */
    frame = (volatile uint32_t *)((char *)prptr->prstkptr -
                                  (10 * sizeof(uint32_t)));
    frame[0] = 0;
    frame[1] = 0;
    frame[2] = 0;
    frame[3] = 0;
    frame[4] = 0;
    frame[5] = 0;
    frame[6] = 0;
    frame[7] = 0;
    frame[8] = (uint32_t)funcaddr;
    frame[9] = (uint32_t)userret;

    prptr->prstkptr = (char *)frame;
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