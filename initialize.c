#include "types.h"
#include "process.h"
#include "io.h"
#include "memory.h"
#include "queue.h"
#include "serial.h"
#include "string.h"
#include "semaphore.h"
#include <ipc.h>

qid16_t	readylist;
qid16_t msgwaitlist;
qid16_t currmsg;

struct procent proctab[NPROC];
struct msgstk msgtable[NMSG];
struct sentry semtab[NSEM];
int32_t prcount;
pid32_t currpid;

void proc_init(void)
{                                           //Temporary
    int32_t i;

    currmsg = 0;
    for (i = 0; i < NPROC; i++) {
        proctab[i].prstate = PR_FREE;
    }

    proctab[NULLPROC].prstate = PR_CURR;
    currpid = NULLPROC;
    prcount = 1;
    //proc_queue_init();
    readylist = newqueue();
    for (i = 0; i < NSEM; i++)
    {
        semtab[i].semstate = SEM_FREE;
       // semtab[i].semcount = 0;
        semtab[i].semqueue = newqueue();
    }
    msgwaitlist = newqueue();
}