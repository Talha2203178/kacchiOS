#ifndef NSEM
#define NSEM		8
#endif

#define SEM_FREE	0
#define SEM_USED	1

struct sentry {
    uint16_t	semstate;	
    int32_t	    semcount;
    qid16_t		semqueue;	
};

extern struct sentry semtab[];

#define isbadsem(x)	( ((int32_t)(x) < 0) || ((int32_t)(x) >= NSEM))