#include "kacchi.h"

static sid32_t newsem(void);

syscall wait(sid32_t sem)
{
    struct procent *prptr;
    struct sentry *semptr;

    if (isbadsem(sem))
    {
        return SYSERR;
    }

    semptr = &semtab[sem];

    if (semptr->sstate == SEM_FREE)
    {
        return SYSERR;
    }

    if (--(semptr->scount) < 0)
    {
        prptr = &proctab[currpid];
        prptr->prstate = PR_WAIT;
        prptr->prsem = sem;
        enqueue(currpid, semptr->semqueue);
        resched();  
    }

    return OK;
}

syscall signal(sid32_t sem)
{
    struct sentry *semptr;


    if (isbadsem(sem))
    {
        return SYSERR;
    }

    semptr = &semtab[sem];

    if (semptr->sstate == SEM_FREE)
    {
        return SYSERR;
    }

    if ((semptr->scount)++ < 0)
    {
        ready(dequeue(semptr->semqueue));
    }

    return OK;
}

sid32_t semcreate(int32_t count)
{
    sid32_t sem;

    if (count < 0)
    {
        return SYSERR;
    }

    sem = newsem();
    if (sem == SYSERR)
    {
        return SYSERR;
    }

    return sem;
}

static sid32_t newsem(void)
{
    static sid32_t nextsem = 0;
    sid32_t sem;

    for (int i = 0; i < NSEM; i++)
    {
        sem = nextsem;
        nextsem = (nextsem + 1) % NSEM;
        if (semtab[sem].sstate == SEM_FREE)
        {
            semtab[sem].sstate = SEM_USED;
            return sem;
        }
    }

    return SYSERR;
}

syscall semdelete(sid32_t sem)
{
    struct sentry *semptr;

    if (isbadsem(sem))
    {
        return SYSERR;
    }

    semptr = &semtab[sem];

    if (semptr->sstate == SEM_FREE)
    {
        return SYSERR;
    }

    while (!isempty(semptr->semqueue))
    {
        ready(getfirst(semptr->semqueue));
    }

    semptr->sstate = SEM_FREE;

    return OK;
}

syscall semreset(sid32_t sem, int32_t count)
{
    struct sentry *semptr;
    qid16_t semqueue;
    pid32_t pid;


    if (isbadsem(sem) || count < 0)
    {
        return SYSERR;
    }

    semptr = &semtab[sem];

    if (semptr->sstate == SEM_FREE)
    {
        return SYSERR;
    }

    while (!isempty(semptr->semqueue))
    {
        ready(getfirst(semptr->semqueue));
    }

    semptr->scount = count;

    return OK;
}
