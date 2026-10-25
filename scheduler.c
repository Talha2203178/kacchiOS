#include "types.h"
#include "process.h"
#include "io.h"
#include "memory.h"
#include "queue.h"
#include "serial.h"
#include "string.h"
#include "semaphore.h"
syscall resched(void)
{
    struct procent *oldproc;
    struct procent *newproc;
    pid32_t nextpid;

    oldproc = &proctab[currpid];
    if (oldproc->prstate == PR_CURR) {
        if (nonempty(readylist) &&
            oldproc->prprio > firstkey(readylist)) {
            return OK;
        }
        oldproc->prstate = PR_READY;
        if (insert(currpid, readylist, oldproc->prprio) == SYSERR) {
            oldproc->prstate = PR_CURR;
            return SYSERR;
        }
    }

    nextpid = dequeue(readylist);
    if (nextpid == EMPTY || nextpid == SYSERR) {
        oldproc->prstate = PR_CURR;
        return SYSERR;
    }

    currpid = nextpid;
    newproc = &proctab[currpid];
    newproc->prstate = PR_CURR;
    ctxsw(&oldproc->prstkptr, newproc->prstkptr);
    return OK;
}

int ready(pid32_t pid)
{
    register struct procent *prptr;

    if (isbadpid(pid)) {
        return SYSERR;
    }

    prptr = &proctab[pid];
    prptr->prstate = PR_READY;
    insert(pid, readylist, prptr->prprio);
    resched();

    return OK;
}