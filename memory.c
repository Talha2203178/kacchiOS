#include <memory.h>
#include "serial.h"
#include "types.h"

struct blockstr freemem; 

void *heapcrt(uint32_t nbytes)
{
    struct blockstr *prev, *curr, *leftover;

    if (0 == nbytes)
    {
        serial_puts("Can not assign 0 bytes in heap\n");
        return NULL;
    }

    /* round to multiple of 8   */
    nbytes = (uint32_t)highbyte(nbytes);

    prev = &freemem;
    curr = freemem.next;
    while (curr != NULL)
    {
        if (curr->size == nbytes)
        {
            serial_puts("Block matched exactly in heap\n");
            prev->next = curr->next;
            freemem.size -= nbytes;

            return (void *)(curr);
        }
        else if (curr->size > nbytes)
        {
            /* split block into two */
            serial_puts("split block into two for heap\n");
            leftover = (struct blockstr *)((uint32_t)curr + nbytes);
            prev->next = leftover;
            leftover->next = curr->next;
            leftover->size = curr->size - nbytes;
            freemem.size -= nbytes;

            return (void *)(curr);
        }
        prev = curr;
        curr = curr->next;
    }
    return NULL;
}


char *heapkill(void *memptr, uint32_t nbytes)
{
    struct blockstr *block, *next, *prev;
    uint32_t top;

    /* make sure block is in heap */
    if ((0 == nbytes)
        || ((uint32_t)memptr < (uint32_t)memsec))
        //|| ((uint32_t)memptr > (uint32_t)32))
    {
        return (char *) "error1\n";
    }

    block = (struct blockstr *)memptr;
    nbytes = (uint32_t)highbyte(nbytes);

    prev = &freemem;
    next = freemem.next;
    while ((next != NULL) && (next < block))
    {
        prev = next;
        next = next->next;
    }

    /* find top of previous memblock */
    if (prev == &freemem)
    {
        top = (uint32_t)NULL;
    }
    else
    {
        top = (uint32_t)prev + prev->size;
    }

    /* make sure block is not overlapping on prev or next blocks */
    if ((top > (uint32_t)block)
        || ((next != NULL) && ((uint32_t)block + nbytes) > (uint32_t)next))
    {
         return (char *) "error2\n";
    }

    freemem.size += nbytes;

    /* coalesce with previous block if adjacent */
    if (top == (uint32_t)block)
    {
        prev->size += nbytes;
        block = prev;
         serial_puts("matched\n");
    }
    else
    {
        block->next = next;
        block->size = nbytes;
        prev->next = block;
         serial_puts("did not match\n");
    }

    /* coalesce with next block if adjacent */
    if (((uint32_t)block + block->size) == (uint32_t)next)
    {
        block->size += next->size;
        block->next = next->next;
         serial_puts("second phase\n");
    }
     return (char *) "done\n";
}



void *stkcrt(uint32_t nbytes)
{
    struct blockstr *prev, *next, *fits, *fitsprev;

    if (0 == nbytes)
    {
        serial_puts("Can not assign 0 bytes in stack\n");
        return (void *)"er1\n";
    }

    /* round to multiple of 8   */
    nbytes = (uint32_t)highbyte(nbytes);

    prev = &freemem;
    next = freemem.next;
    fits = NULL;
    fitsprev = NULL;

    /* scan list for highest block that fits */
    while (next != NULL)
    {
        if (next->size >= nbytes)
        {
            fits = next;
            fitsprev = prev;
        }
        prev = next;
        next = next->next;
    }

    if (NULL == fits)
    {
        return (void *)"er2\n";
    }

    if (nbytes == fits->size)
    {
        serial_puts("Block matched exactly in stack\n");
        fitsprev->next = fits->next;
    }
    else
    {
        serial_puts("take top portion for stack\n");
        fits->size -= nbytes;
        fits = (struct blockstr *)((uint32_t)fits + fits->size);
    }

    freemem.size -= nbytes;
    return (void *)((uint32_t)fits + nbytes - sizeof(int));  //subtract sizeof(int) because, it gives address of last byte.
}