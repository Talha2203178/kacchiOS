#include "types.h"
#include "process.h"

pri16_t resume(pid32_t pid)
{
    struct procent *prptr;

    if (isbadpid(pid))
    {
        return SYSERR;
    }

    prptr = &proctab[pid];

    if (prptr->prstate == PR_SUSP)
    {
        prptr->prstate = PR_READY;
    }
    else
    {
        return SYSERR;
    }

    return OK;
}