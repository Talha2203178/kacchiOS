#include "process.h" //Temporary process runner

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

    prptr->prentry();

    /*
     * The test process returned normally.
     * Treat that as process termination.
     */
    return kill(pid);
}