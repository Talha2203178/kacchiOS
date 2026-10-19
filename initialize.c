#include "process.h"

struct procent proctab[NPROC];
int32_t prcount;
pid32_t currpid;

void proc_init(void)
{                                           //Temporary
    int32_t i;

    for (i = 0; i < NPROC; i++) {
        proctab[i].prstate = PR_FREE;
    }

    proctab[NULLPROC].prstate = PR_CURR;
    currpid = NULLPROC;
    prcount = 1;
}