#ifndef __GC_H__
#define __GC_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define ASSERT_NOT_NULL(p) {if (NULL == p) {printf("ASSERT_NOT_NULL %s:%d: %s\n", __FILE__, __LINE__, #p);}}
#ifdef GC_TRACE
#define GC_TRACE_OUT(...) printf(__VA_ARGS__);
#else
#define GC_TRACE_OUT(...) //
#endif

typedef struct GCNode
{
    GCNode* prev;
    GCNode* next;
    void* address;
    uint32_t size;
    bool reachable;
} GCNode;

void* operator new(unsigned int size);

#define gc_init() {char* __ebp; asm volatile("movl %%ebp, %0" : "=g"(__ebp));gc_init_internal(__ebp); }
void gc_init_internal(char* ebp);
void gc_mark();
void gc_sweep();
void gc();


#endif // __GC_H__
