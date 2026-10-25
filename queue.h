#ifndef QUEUE_H
#define QUEUE_H

#include "kacchi.h"

#ifndef NQENT
#define NQENT          (NPROC + 4 + NSEM + NSEM)
#endif

#define EMPTY          (-1)
#define MAXKEY         0x7FFFFFFF
#define MINKEY         0x80000000

struct quentry {
    int32_t qkey;
    qid16_t qnext;
    qid16_t qprev;
};

extern struct quentry qt[];
extern	qid16_t	readylist;
extern qid16_t waitlist;
extern qid16_t semlist;

#define queuehead(q)   (q)
#define queuetail(q)   ((q) + 1)
#define firstid(q)     (qt[queuehead(q)].qnext)
#define lastid(q)      (qt[queuetail(q)].qprev)
#define isempty(q)     (firstid(q) >= NPROC)
#define nonempty(q)    (firstid(q) < NPROC)
#define firstkey(q)    (qt[firstid(q)].qkey)
#define lastkey(q)     (qt[lastid(q)].qkey)

#define isbadqid(x)    ((int32_t)(x) < 0 || (int32_t)(x) >= NQENT - 1)

pid32_t getfirst(qid16_t q);
pid32_t getlast(qid16_t q);
pid32_t getitem(pid32_t pid);
pid32_t enqueue(pid32_t pid, qid16_t q);
pid32_t dequeue(qid16_t q);
int insert(pid32_t pid, qid16_t q, int32_t key);
qid16_t newqueue(void);

#endif