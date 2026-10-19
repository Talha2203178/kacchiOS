#include "process.h"

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