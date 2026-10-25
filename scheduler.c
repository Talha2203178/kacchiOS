#include "kacchi.h"
#include "queue.h"

#define WAITQ       (NPROC + 2)
#define SEMQ(sem)   (NPROC + 4 + ((sem) * 2))

qid16_t readylist;
char *sched_sp;
char *proc_sp;

void proc_queue_init(void)
{
    qid16_t queue;

    readylist = newqueue();
    newqueue();
    for (queue = 0; queue < NSEM; queue++) {
        newqueue();
    }
}

void ready_enqueue(pid32_t pid)
{
    if (!isbadpid(pid)) {
        insert(pid, readylist, proctab[pid].prprio);
    }
}

void wait_enqueue(pid32_t pid)
{
    if (!isbadpid(pid)) {
        enqueue(pid, WAITQ);
        proctab[pid].prstate = PR_WAIT;
    }
}

void sem_enqueue(sid32_t sem, pid32_t pid)
{
    if (sem < 0 || sem >= NSEM || isbadpid(pid)) {
        return;
    }

    enqueue(pid, SEMQ(sem));
    proctab[pid].prsem = sem;
    proctab[pid].prstate = PR_WAIT;
}

pid32_t sem_dequeue(sid32_t sem)
{
    pid32_t pid;

    if (sem < 0 || sem >= NSEM) {
        return SYSERR;
    }

    pid = dequeue(SEMQ(sem));
    if (pid >= 0) {
        proctab[pid].prsem = -1;
        proctab[pid].prstate = PR_READY;
    }
    return pid;
}

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
