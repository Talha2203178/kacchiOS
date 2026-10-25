/* types.h - Basic type definitions */
#ifndef TYPES_H
#define TYPES_H

typedef unsigned int   uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char  uint8_t;
typedef int            int32_t;
typedef short          int16_t;
typedef short         qid16_t;
typedef char           int8_t;

typedef uint32_t size_t;
typedef int32_t  pid32_t;
typedef int16_t  pri16_t;
typedef int32_t  sid32_t;
typedef int32_t  umsg32_t;
typedef uint8_t  bool8_t;
typedef int32_t  syscall;

#define NULL  ((void*)0)
#define TRUE  1
#define FALSE 0
#define OK    0
#define SYSERR ((int32_t)-1)

#endif