#include "kacchi.h"
#include "queue.h"

struct quentry qt[NQENT];

pid32_t getfirst(qid16_t q)
{
    if (isbadqid(q)) {
        return SYSERR;
    }
    return firstid(q);
}

pid32_t getlast(qid16_t q)
{
    if (isbadqid(q)) {
        return SYSERR;
    }
    return lastid(q);
}

pid32_t getitem(pid32_t pid)
{
    pid32_t prev;
    pid32_t next;

    if (pid < 0 || pid >= NPROC) {
        return SYSERR;
    }

    prev = qt[pid].qprev;
    next = qt[pid].qnext;
    if (prev < 0 || next < 0) {
        return SYSERR;
    }

    qt[prev].qnext = (qid16_t)next;
    qt[next].qprev = (qid16_t)prev;
    qt[pid].qnext = EMPTY;
    qt[pid].qprev = EMPTY;
    return pid;
}

pid32_t enqueue(pid32_t pid, qid16_t q)
{
    qid16_t tail;
    qid16_t prev;

    if (isbadqid(q) || isbadpid(pid)) {
        return SYSERR;
    }

    tail = queuetail(q);
    prev = qt[tail].qprev;
    qt[pid].qnext = tail;
    qt[pid].qprev = prev;
    qt[pid].qkey = MAXKEY;
    qt[prev].qnext = pid;
    qt[tail].qprev = pid;
    return pid;
}

pid32_t dequeue(qid16_t q)
{
    pid32_t pid;

    if (isbadqid(q)) {
        return SYSERR;
    }
    if (isempty(q)) {
        return EMPTY;
    }

    pid = firstid(q);
    getitem(pid);
    return pid;
}

int insert(pid32_t pid, qid16_t q, int32_t key)
{
    qid16_t curr;
    qid16_t prev;

    if (isbadqid(q) || isbadpid(pid)) {
        return SYSERR;
    }

    curr = firstid(q);
    while (curr < NPROC && qt[curr].qkey >= key) {
        curr = qt[curr].qnext;
    }
    prev = qt[curr].qprev;
    qt[pid].qkey = key;
    qt[pid].qnext = curr;
    qt[pid].qprev = prev;
    qt[prev].qnext = pid;
    qt[curr].qprev = pid;
    return TRUE;
}

qid16_t newqueue(void)
{
    static qid16_t nextqid = NPROC;
    qid16_t q;

    q = nextqid;
    if (q < 0 || q + 1 >= NQENT) {
        return SYSERR;
    }
    nextqid += 2;

    qt[queuehead(q)].qnext = queuetail(q);
    qt[queuehead(q)].qprev = EMPTY;
    qt[queuehead(q)].qkey = MAXKEY;
    qt[queuetail(q)].qnext = EMPTY;
    qt[queuetail(q)].qprev = queuehead(q);
    qt[queuetail(q)].qkey = MINKEY;
    return q;
}