#include <memory.h>
#include <serial.h>
#include <types.h>
#include <queue.h>
#include <ipc.h>
#include <process.h>
#include <interrupts.h>

struct msgstk rcvmsg(void){
    struct msgstk stk;
    qid16_t i;
    qid16_t currtab;
    stk.mypid = currpid;
    uint32_t mask = irq_save();
    if(currmsg < 0 || currmsg > NMSG){
        serial_puts("Invalid message queue\n");
        irq_restore(mask);
        return stk;
    }
    for(i = 0; i < NMSG; i++){
        if(msgtable[i].mypid == stk.mypid){
            serial_puts("Message can be sent only once\n");
            irq_restore(mask);
            return stk;
        }
    }
    msgtable[currmsg++] = stk;
    struct procent *prptr;
    prptr = &proctab[currpid];
    prptr->prstate = PR_MSGWAIT;
    enqueue(currpid, msgwaitlist);
    resched();
    for(i = 0; i < NMSG; i++){
        if(msgtable[i].mypid == stk.mypid){
            currtab = i;
        }
    }
    stk = msgtable[currtab];
    for(i = currtab+1; i < currmsg; i++){
        msgtable[i-1] = msgtable[i];
    }
    currmsg--;
    irq_restore(mask);
    return stk;
}

void *sendmsg(char *msg, uint32_t msglen, pid32_t id){
    char *stk = (char *)stkcrt(msglen);
    char *iterator;
    pid32_t pid;
    uint32_t mask = irq_save();
    iterator = stk;

    if (stk == NULL) {
        irq_restore(mask);
        return NULL;
    }

    qid16_t i;
    for(i = 0; i < msglen; i++){
        *iterator = msg[i];
        iterator++;
    }
    if(isempty(msgwaitlist)){
        serial_puts("No process is waiting for message\n");
        irq_restore(mask);
        return NULL;
    }
    if(id > NPROC){
        pid = dequeue(msgwaitlist);
    }
    else{
        serial_puts("Here i am floating in my tin can");
        pid = getitem(id);
    }
    for(i = 0; i < NMSG; i++){
        if(msgtable[i].mypid == pid){
            msgtable[i].stkbase = (uint32_t *)stk;
           // msgtable[i].stkptr = (uint32_t *)iterator;
            msgtable[i].nobytes = msglen;
            ready(pid);
            irq_restore(mask);
            return OK;
        }
    }
    irq_restore(mask);
    return NULL;
}
