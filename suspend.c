#include "types.h"
#include "process.h"

syscall suspend(pid32_t pid)
{
    struct procent *prptr;

    if (isbadpid(pid))
    {
        return SYSERR;
    }

    prptr = &proctab[pid];

    if (prptr->prstate == PR_CURR)
    {
        prptr->prstate = PR_SUSP;
    }
    else if (prptr->prstate == PR_READY)
    {
        prptr->prstate = PR_SUSP;
    }
    else
    {
        return SYSERR;
    }

    return OK;
}