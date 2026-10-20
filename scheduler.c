#include "process.h"

static struct queuetable queues;
char *sched_sp;
char *proc_sp;

static void enqueue_fifo(struct pidqueue *queue, pid32_t pid)
{
    if (queue->count < NPROC) {
        queue->pids[queue->count++] = pid;
    }
}

static pid32_t dequeue_front(struct pidqueue *queue)
{
    pid32_t pid;
    int32_t index;

    if (queue->count == 0) {
        return SYSERR;
    }

    pid = queue->pids[0];
    for (index = 1; index < queue->count; index++) {
        queue->pids[index - 1] = queue->pids[index];
    }
    queue->count--;

    return pid;
}

void ready_enqueue(pid32_t pid)
{
    int32_t index;

    if (queues.ready.count >= NPROC) {
        return;
    }

    index = queues.ready.count;
    while (index > 0 &&
           proctab[queues.ready.pids[index - 1]].prprio < proctab[pid].prprio) {
        queues.ready.pids[index] = queues.ready.pids[index - 1];
        index--;
    }

    queues.ready.pids[index] = pid;
    queues.ready.count++;
}

void wait_enqueue(pid32_t pid)
{
    enqueue_fifo(&queues.wait, pid);
    proctab[pid].prstate = PR_WAIT;
}

void sem_enqueue(sid32_t sem, pid32_t pid)
{
    if (sem < 0 || sem >= NSEM) {
        return;
    }

    enqueue_fifo(&queues.sem[sem], pid);
    proctab[pid].prsem = sem;
    proctab[pid].prstate = PR_WAIT;
}

pid32_t sem_dequeue(sid32_t sem)
{
    pid32_t pid;

    if (sem < 0 || sem >= NSEM) {
        return SYSERR;
    }

    pid = dequeue_front(&queues.sem[sem]);
    if (pid != SYSERR) {
        proctab[pid].prsem = -1;
        proctab[pid].prstate = PR_READY;
    }

    return pid;
}

pid32_t next_process(void)
{
    return dequeue_front(&queues.ready);
}

syscall resched(void)
{
    pid32_t nextpid;
    struct procent *prptr;

    nextpid = next_process();
    if (nextpid == SYSERR) {
        return SYSERR;
    }

    prptr = &proctab[nextpid];
    currpid = nextpid;
    prptr->prstate = PR_CURR;
    proc_sp = prptr->prstkptr;

    ctxsw(&sched_sp, proc_sp);
    return OK;
}
