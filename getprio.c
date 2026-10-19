#include "process.h"

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