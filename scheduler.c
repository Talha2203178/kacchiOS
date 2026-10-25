#include "queue.h"

struct quentry qt[NQENT];
char *sched_sp;
char *proc_sp;

#define READYQ          (NPROC)
#define WAITQ           (NPROC + 2)
#define SEMQ(sem)       (NPROC + 4 + ((sem) * 2))

static void queue_init(qid16_t queue)
{
    qt[queuehead(queue)].qnext = queuetail(queue);
    qt[queuetail(queue)].qprev = queuehead(queue);
}

static void enqueue(qid16_t queue, pid32_t pid, int32_t key)
{
    qid16_t current;
    qid16_t previous;

    current = firstid(queue);
    previous = queuehead(queue);
    while (current < NPROC && qt[current].qkey >= key) {
        previous = current;
        current = qt[current].qnext;
    }

    qt[pid].qkey = key;
    qt[pid].qprev = previous;
    qt[pid].qnext = current;
    qt[previous].qnext = (qid16_t)pid;
    if (current < NPROC) {
        qt[current].qprev = (qid16_t)pid;
    } else {
        qt[queuetail(queue)].qprev = (qid16_t)pid;
    }
}

static pid32_t dequeue(qid16_t queue, bool8_t ready_only)
{
    qid16_t pid;
    qid16_t next;

    while (nonempty(queue)) {
        pid = firstid(queue);
        next = qt[pid].qnext;
        qt[queuehead(queue)].qnext = next;
        qt[next].qprev = queuehead(queue);
        if (pid < NPROC && (!ready_only || proctab[pid].prstate == PR_READY)) {
            return pid;
        }
    }
    return SYSERR;
}

void proc_queue_init(void)
{
    qid16_t queue;

    for (queue = NPROC; queue < NQENT; queue += 2) {
        queue_init(queue);
    }
}

void ready_enqueue(pid32_t pid)
{
    if (!isbadpid(pid)) {
        enqueue(READYQ, pid, proctab[pid].prprio);
    }
}

void wait_enqueue(pid32_t pid)
{
    if (!isbadpid(pid)) {
        enqueue(WAITQ, pid, 0);
        proctab[pid].prstate = PR_WAIT;
    }
}

void sem_enqueue(sid32_t sem, pid32_t pid)
{
    if (sem < 0 || sem >= NSEM || isbadpid(pid)) {
        return;
    }

    enqueue(SEMQ(sem), pid, 0);
    proctab[pid].prsem = sem;
    proctab[pid].prstate = PR_WAIT;
}

pid32_t sem_dequeue(sid32_t sem)
{
    pid32_t pid;

    if (sem < 0 || sem >= NSEM) {
        return SYSERR;
    }

    pid = dequeue(SEMQ(sem), FALSE);
    if (pid != SYSERR) {
        proctab[pid].prsem = -1;
        proctab[pid].prstate = PR_READY;
    }

    return pid;
}

pid32_t next_process(void)
{
    return dequeue(READYQ, TRUE);
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
