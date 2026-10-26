#include <types.h>
#ifndef NSEM
#define NSEM		2
#endif

#define SEM_FREE	0
#define SEM_USED	1

struct sentry {
    uint16_t	semstate;	
    int32_t	    semcount;
    qid16_t		semqueue;	
};

extern struct sentry semtab[NSEM];

#define isbadsem(x)	( ((int32_t)(x) < 0) || ((int32_t)(x) >= NSEM))
syscall wait(sid32_t sem);
syscall signal(sid32_t sem);
sid32_t semcreate(int32_t count);
 sid32_t newsem(void);
syscall semdelete(sid32_t sem);
syscall semreset(sid32_t sem, int32_t count);