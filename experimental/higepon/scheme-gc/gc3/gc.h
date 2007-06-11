#ifndef __GC_H__
#define __GC_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#ifndef MONA
#include <unistd.h>
#endif
#include "mysetjmp.h"
#define GC_ASSERT_NOT_NULL(p) {if (NULL == p) {printf("GC_ASSERT_NOT_NULL %s:%d: %s\n", __FILE__, __LINE__, #p);exit(-1);}}
#define GC_ASSERT(p) {if (!(p)) {printf("GC_ASSERT %s:%d: %s\n", __FILE__, __LINE__, #p);fflush(stdout);exit(-1);}}
#ifdef GC_TRACE
#ifdef MONA
#include "monapi.h"
#define GC_TRACE_OUT(...) _logprintf(__VA_ARGS__);
#else
#define GC_TRACE_OUT(...) printf(__VA_ARGS__);fflush(stdout);
#endif
#else
#define GC_TRACE_OUT(...) //
#endif
#define GC_SAFE_POINTER(a) (((uint32_t)a >> 16) | (((uint32_t)a & 0x0000ffff) << 16))

extern "C" char  __bss_start[];
extern "C" char  _end[];

// padding に注意
// 4byte align を前提に mark_heap する
#pragma pack(4)
typedef struct GCRecord
{
    GCRecord* prev;
    GCRecord* next;
    uint32_t size;
    uint32_t magic;
    uint32_t reachable  : 1;
    uint32_t haspointer : 1;
    uint32_t checkdone  : 1;
    uint32_t age      : 29;
    char data[0];
} GCRecord;
#pragma pack()

#ifdef MONA
extern "C" char  _data_start__[];
extern "C" char  _bss_end__[];
#define gc_init() {                                \
    char* __ebp;                                   \
    asm volatile("movl %%ebp, %0" : "=g"(__ebp));  \
    gc_init_internal(__ebp, _data_start__, _bss_end__);    \
    }
#else
extern "C" char  __bss_start[];
extern "C" char  _end[];
#define gc_init() {                                \
    char* __ebp;                                   \
    asm volatile("movl %%ebp, %0" : "=g"(__ebp));  \
    gc_init_internal(__ebp, __bss_start, _end);    \
}
#endif

void gc();
void gc_init_internal(char* stack_bottom, char* data_start, char* data_end);
void gc_fini();
extern "C" void* gc_malloc(uint32_t size, bool haspointer = true);
extern "C" void* gc_realloc(void* p, size_t size);
extern "C" void* gc_calloc(size_t n, size_t size);
extern "C" void gc_dont_free(void* p);

void* operator new(unsigned int size);
void* operator new(unsigned int size, bool haspointer);
void* operator new[](unsigned int size);
void* operator new[](unsigned int size, bool haspointer);

#endif // __GC_H__
