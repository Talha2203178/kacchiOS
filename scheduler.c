#include "process.h"

static pid32_t ready_queue[NPROC];
static int32_t ready_count;
char *sched_sp;
char *proc_sp;

void ready_enqueue(pid32_t pid)
{
    int32_t index;

    if (ready_count >= NPROC) {
        return;
    }

    index = ready_count;
    while (index > 0 &&
           proctab[ready_queue[index - 1]].prprio < proctab[pid].prprio) {
        ready_queue[index] = ready_queue[index - 1];
        index--;
    }

    ready_queue[index] = pid;
    ready_count++;
}

pid32_t next_process(void)
{
    pid32_t selected;
    int32_t index;

    if (ready_count == 0) {
        return SYSERR;
    }

    selected = ready_queue[0];
    for (index = 1; index < ready_count; index++) {
        ready_queue[index - 1] = ready_queue[index];
    }
    ready_count--;

    return selected;
}
