#ifndef __GC_H__
#define __GC_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "mysetjmp.h"
#define GC_ASSERT_NOT_NULL(p) {if (NULL == p) {printf("GC_ASSERT_NOT_NULL %s:%d: %s\n", __FILE__, __LINE__, #p);}}
#define GC_ASSERT(p) {if (!p) {printf("GC_ASSERT %s:%d: %s\n", __FILE__, __LINE__, #p);}}
#ifdef GC_TRACE
#define GC_TRACE_OUT(...) printf(__VA_ARGS__);
#else
#define GC_TRACE_OUT(...) //
#endif
#define GC_SAFE_POINTER(a) (((uint32_t)a >> 16) | (((uint32_t)a & 0x0000ffff) << 16))

extern "C" char  __bss_start[];
extern "C" char  _end[];

// padding に注意
typedef struct GCRecord
{
    GCRecord* prev;
    GCRecord* next;
    uint32_t size;
    uint32_t magic;
    uint32_t reachable  : 1;
    uint32_t haspointer : 1;
    uint32_t checkdone  : 1;
    uint32_t dummy      : 29;
    char data[0];
} GCRecord;

#define gc_init() {                                \
    char* __ebp;                                   \
    asm volatile("movl %%ebp, %0" : "=g"(__ebp));  \
    gc_init_internal(__ebp, __bss_start, _end);    \
}

void gc();
void gc_init_internal(char* stack_bottom, char* data_start, char* data_end);
void* gc_malloc(uint32_t size);
void* operator new(unsigned int size);

#endif // __GC_H__
