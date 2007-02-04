#ifndef __CONT_H__
#define __CONT_H__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mysetjmp.h"

typedef struct Cont
{
    uint32_t stack_size;
    uint8_t* stack;
    myjmp_buf registers;
};

void cont_initialize();
int cont_save(Cont* c);
void cont_restore(Cont* c);
void* cont_get_stack_pointer();
void cont_destroy(Cont* c);

#endif // __CONT_H__
