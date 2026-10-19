#ifndef _MEMORY_H_
#define _MEMORY_H_
#include "types.h"

//make no of bits bytes by making them multiple of 8
#define highbyte(x)  (void *)( (7 + (uint32_t)(x)) & ~7 )
#define lowbyte(x)  (void *)( ((uint32_t)(x)) & ~7 )


#define stkkill(p, len) heapkill((void *)((uint32_t)(p) - (uint32_t)highbyte(len) + (uint32_t)sizeof(uint32_t)), (uint32_t)highbyte(len))  //sizeof is operator in c

struct blockstr
{
    struct blockstr *next;          /*pointer to next memory block       */
    uint32_t size;                    /**< size of memory block including this structure */
};

extern struct blockstr freemem;     /**< head of free memory list           */


extern char memsec[];          //heap and stack section

/* Memory functions */
void *heapcrt(uint32_t);
char *heapkill(void *, uint32_t);
void *stkcrt(uint32_t);

#endif                    