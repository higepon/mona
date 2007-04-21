/*!
    COPYRIGHT AND PERMISSION NOTICE

    Copyright (c) 2007 higepon@users.sourceforge.jp

    All rights reserved.

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
    deal in the Software without restriction, including without limitation the
    rights to use, copy, modify, merge, publish, distribute, and/or sell copies
    of the Software, and to permit persons to whom the Software is furnished to
    do so, provided that the above copyright notice(s) and this permission
    notice appear in all copies of the Software and that both the above
    copyright notice(s) and this permission notice appear in supporting
    documentation.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF THIRD PARTY RIGHTS.
    IN NO EVENT SHALL THE COPYRIGHT HOLDER OR HOLDERS INCLUDED IN THIS NOTICE BE
    LIABLE FOR ANY CLAIM, OR ANY SPECIAL INDIRECT OR CONSEQUENTIAL DAMAGES, OR
    ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
    IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
    OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

    Except as contained in this notice, the name of a copyright holder shall not
    be used in advertising or otherwise to promote the sale, use or other
    dealings in this Software without prior written authorization of the
    copyright holder.
*/

/*!
    \file   cont.c
    \brief  cotinuation library

    Copyright (c) 2007 higepon
    All rights reserved.
    License=MIT/X License

    \author  higepon
*/

#include "cont.h"
#ifndef MONA
#include <unistd.h>
#endif

static uint32_t cont_stack_bottom = (uint32_t)NULL;
static Cont* c = NULL;
static int r   = 0;
static uint32_t diff       = 0;
static uint32_t prev_stack = 0;
static uint32_t next_stack = 0;;

void* cont_get_stack_pointer()
{
    register void* stack_pointer asm ("%esp");
    return (void*)((unsigned int)stack_pointer + 8);
}

#ifdef MONA
void* cont_get_stack_bottom()
{
  return (void*)0xF0000000;
}

#else
#pragma weak __libc_stack_end
extern void* __libc_stack_end;
void* cont_get_stack_bottom()
{
    long pagesize = sysconf(_SC_PAGESIZE);
    return (void*)(((uintptr_t)__libc_stack_end + pagesize) & ~(pagesize - 1));
}
#endif

void cont_initialize()
{
    // fix me!
#ifdef MONA
    cont_stack_bottom = (uint32_t)cont_get_stack_pointer();
#else
    cont_stack_bottom = (uint32_t)cont_get_stack_pointer() + 150;
#endif
    c = NULL;
    r = 0;
    diff = 0;
    prev_stack = 0;
    next_stack = 0;
    //  cont_stack_bottom = cont_get_stack_bottom() -20;
}

void cont_destroy(Cont* c)
{
    if (c->stack != NULL)
    {
        free(c->stack);
        c->stack = NULL;
    }
}

void cont_restore(Cont* cc, int return_value)
{
    c = cc;
    r = return_value;
    uint32_t i;
    prev_stack = c->registers[7];
    next_stack= prev_stack - 1000;
    for (i = 0; i < c->stack_size / 4;  i++)
    {
        uint32_t* p = (uint32_t*)c->stack;
        if (prev_stack <= p[i] && p[i] <= cont_stack_bottom)
        {
            p[i] -= (prev_stack - next_stack);
        }
    }

    // eax ebx ecx edx esi edi
    for (i = 0; i < 5; i++)
    {
        if (prev_stack <= c->registers[i] && c->registers[i] <= cont_stack_bottom)
        {
            c->registers[i] -= (c->registers[i] - next_stack);
        }
    }
    memcpy((uint8_t*)next_stack, c->stack, c->stack_size);
    diff = c->registers[6] - c->registers[7];
    c->registers[7] = next_stack;
    c->registers[6] = next_stack + diff;
    mylongjmp(c->registers, r);
}

int cont_save(Cont* c)
{
    int ret = mysetjmp(c->registers);
    if (ret != 0) return ret;

    uint32_t current_stack = c->registers[7];
    c->stack_size = cont_stack_bottom - current_stack;
#ifdef USE_BOEHM_GC
    c->stack = (uint8_t*)GC_MALLOC(c->stack_size);
#else
    c->stack = (uint8_t*)malloc(c->stack_size);
#endif
    memcpy(c->stack, (uint8_t*)current_stack, c->stack_size);
    return ret;
}
