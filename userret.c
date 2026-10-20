#include "process.h"

void userret(void)
{
    kill(currpid);
    ctxsw(&proc_sp, sched_sp);

    for (;;) {
    }
}