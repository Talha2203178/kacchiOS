#include "types.h"
#include "process.h"
#include "memory.h"

int32_t kill(pid32_t pid)
{
    struct procent *prptr;

    if (isbadpid(pid) || pid == NULLPROC) {
        return SYSERR;
    }

    prptr = &proctab[pid];

    if (prptr->prstate != PR_CURR &&
        prptr->prstate != PR_READY &&
        prptr->prstate != PR_SUSP) {
        return SYSERR;
    }

    // if (stkkill(prptr->prstkptr, prptr->prstklen) == SYSERR) {
    //     return SYSERR;
    // }

    prptr->prstate = PR_FREE;
    prptr->prstkptr = NULL;
    prptr->prstkbase = NULL;
    prptr->prstklen = 0;
    prptr->prentry = NULL;

    if (prcount > 0) {
        prcount--;
    }

    if (currpid == pid) {
        currpid = NULLPROC;
    }

    return OK;
}