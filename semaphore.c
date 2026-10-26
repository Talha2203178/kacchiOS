#include "types.h"
#include "process.h"
#include "io.h"
#include "memory.h"
#include "queue.h"
#include "serial.h"
#include "string.h"
#include "semaphore.h"

 sid32_t newsem(void);

syscall wait(sid32_t sem)
{
    struct procent *prptr;
    struct sentry *semptr;

    if (isbadsem(sem))
    {
        return SYSERR;
    }

    semptr = &semtab[sem];

    if (semptr->semstate == SEM_FREE)
    {
        return SYSERR;
    }

    if (--(semptr->semcount) < 0)
    {
        prptr = &proctab[currpid];
        prptr->prstate = PR_WAIT;
        prptr->prsem = sem;
        enqueue(currpid, semptr->semqueue);
        serial_puts("\nin wait\n");
        resched();  
    }

    return OK;
}

syscall signal(sid32_t sem)
{
    struct sentry *semptr;
    pid32_t i;

    if (isbadsem(sem))
    {
        return SYSERR;
    }

    semptr = &semtab[sem];

    if (semptr->semstate == SEM_FREE)
    {
        return SYSERR;
    }

    if ((semptr->semcount)++ < 0)
    {
        serial_puts("\nin signal\n");
        i = dequeue(semptr->semqueue);
        serial_puts("\nin signal after dequeue\n");
        ready(i);
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
    semtab[sem].semcount = count;
    return sem;
}

 sid32_t newsem(void)
{
    static sid32_t nextsem = 0;
    sid32_t sem;

    for (int i = 0; i < NSEM; i++)
    {
        sem = nextsem;
        nextsem = (nextsem + 1) % NSEM;
        if (semtab[sem].semstate == SEM_FREE)
        {
            semtab[sem].semstate = SEM_USED;
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

    if (semptr->semstate == SEM_FREE)
    {
        return SYSERR;
    }

    while (!isempty(semptr->semqueue))
    {
        ready(getfirst(semptr->semqueue));
    }

    semptr->semstate = SEM_FREE;

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

    if (semptr->semstate == SEM_FREE)
    {
        return SYSERR;
    }

    while (!isempty(semptr->semqueue))
    {
        ready(getfirst(semptr->semqueue));
    }

    semptr->semcount = count;

    return OK;
}
