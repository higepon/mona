#ifndef __CONT_H__
#define __CONT_H__

#ifdef USE_BOEHM_GC
#include "gc.h"
#endif

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mysetjmp.h"

typedef struct Cont
{
    uint32_t stack_size;
    uint32_t stack_bottom;
    uint8_t* stack;
    myjmp_buf registers;
} Cont;


#ifdef USE_MONA_GC
extern void* gc_malloc_has_pointer(uint32_t size);
#endif
void cont_initialize();
int cont_save(Cont* c);
void cont_restore(Cont* c, int return_value);
void* cont_get_stack_pointer();
void* cont_get_stack_bottom();
void cont_destroy(Cont* c);

#endif // __CONT_H__
