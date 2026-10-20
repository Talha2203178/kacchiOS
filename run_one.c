#include "process.h"

int32_t run_one(pid32_t pid)
{
    struct procent *prptr;

    if (isbadpid(pid)) {
        return SYSERR;
    }

    prptr = &proctab[pid];

    if (prptr->prstate != PR_READY) {
        return SYSERR;
    }

    currpid = pid;
    prptr->prstate = PR_CURR;
    proc_sp = prptr->prstkptr;

    ctxsw(&sched_sp, proc_sp);
    return OK;
}