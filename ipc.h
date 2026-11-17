#include <memory.h>
#include <serial.h>
#include <types.h>
#include <queue.h>
#include <process.h>

#ifndef NMSG
#define NMSG 5
#endif

struct msgstk {
    //uint32_t *stkptr;
    uint32_t nobytes;
    uint32_t *stkbase;
    pid32_t mypid;
};

extern qid16_t currmsg;
extern struct msgstk msgtable[NMSG];
struct msgstk rcvmsg(void);
void *sendmsg(char *msg, uint32_t msglen, pid32_t id);