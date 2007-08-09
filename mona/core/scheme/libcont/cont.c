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

#define STACK_FOR_RESTORE 200

static uint32_t system_stack_bottom = (uint32_t)NULL;
static uint32_t current_stack_bottom = (uint32_t)NULL;

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
    /* fix me! */
#ifdef MONA
    system_stack_bottom = (uint32_t)cont_get_stack_pointer();
#else
    system_stack_bottom = (uint32_t)cont_get_stack_pointer() + 150;
#endif
    current_stack_bottom = system_stack_bottom;
}

void cont_destroy(Cont* c)
{
    if (c->stack != NULL)
    {
        free(c->stack);
        c->stack = NULL;
    }
}

int cont_stack_expander(int i)
{
    if (i < 0)
    {
        return i;
    }
    cont_stack_expander(--i);
    return 0;
}

/*

    There are four Continuation restore patterns.
    (1)

             0x00000000 ==> +-----------+
                            |           |
                            |           |
                            |           |
                            |           |
                            |           |   /+----------+
                            |           |  / |          |
          restore point ==> |-----------| /  |          |
                            |           |/   |   Cont   |
                            |           |    |          |
                            |-----------|    |          |
                            |           |    +----------+
                            |           |   /
                            |           |  /  <=== restore!
                            |           | /
                            |           |/
                            |-----------|
    system stack bottom ==> |           |
                            |           |
                            |           |
             0xffffffff ==> +-----------+

    (2)
                                            /+-----------+
                                           / |           |
             0x00000000 ==> +-----------+ /  |           |
                            |           |/   |           |
                            |-----------|    |   Cont    |
                            |           |    |           |
                            |           |    |           |
                            |           |    |           |
                            |           |    +-----------+
                            |           |   /
                            |           |  /
                            |           | /   <=== restore!
                            |           |/
          restore point ==> |-----------|
                            |           |
                            |           |
                            |           |
                            |           |
                            |-----------|
    system stack bottom ==> |           |
                            |           |
                            |           |
             0xffffffff ==> +-----------+


    (3)
             0x00000000 ==> +-----------+
                            |           |
                            |           |
                            |           |
                            |           |
                            |           |
                            |           |   /+----------+
          restore point ==> |-----------|  / |          |
                            |           | /  |   Cont   |
                            |           |/   |          |
                            |           |    |          |
   current stack bottom ==> |-----------|    +----------+
                            |           |   /
                            |           |  /
                            |           | /  <== restore!
                            |           |/
                            |-----------|
    System stack bottom ==> |           |
                            |           |
                            |           |
             0xffffffff ==> +-----------+

    (4)
                                            /+-----------+
                                           / |           |
                                          /  |           |
             0x00000000 ==> +-----------+/   |           |
                            |           |    |   Cont    |
                            |           |    |           |
                            |           |    |           |
                            |           |    |           |
                            |           |    +-----------+
                            |           |   /
                            |           |  /
                            |           | /   <=== restore!
                            |           |/
          restore point ==> |-----------|
                            |           |
                            |           |
                            |           |
   current stack bottom ==> |-----------|
                            |           |
                            |-----------|
    System stack bottom ==> |           |
                            |           |
                            |           |
             0xffffffff ==> +-----------+

*/
void cont_restore(Cont* c, int r)
{
    uint32_t i;
    uint32_t prev_stack = c->registers[7];
    register void* stack_pointer asm ("%esp");
    uint32_t next_stack;
    if (system_stack_bottom == current_stack_bottom)
    {
        /* pattern 1 */
        if (system_stack_bottom - ((uint32_t)stack_pointer + STACK_FOR_RESTORE) > c->stack_size)
        {
            next_stack = (uint32_t)stack_pointer + STACK_FOR_RESTORE;
        }
        /* pattern 2 */
        else
        {
            next_stack = (uint32_t)stack_pointer - (c->stack_size + STACK_FOR_RESTORE);
        }
    }
    else
    {
        /* pattern 3 */
        if (system_stack_bottom - ((uint32_t)stack_pointer + STACK_FOR_RESTORE) > c->stack_size)
        {
            next_stack = system_stack_bottom - c->stack_size;
        }
        /* patter 4 */
        else
        {
            next_stack = (uint32_t)stack_pointer - (c->stack_size + STACK_FOR_RESTORE);
        }
    }

    for (i = 0; i < c->stack_size / 4;  i++)
    {
        uint32_t* p = (uint32_t*)c->stack;
        if (prev_stack <= p[i] && p[i] <= c->stack_bottom)
        {
            p[i] -= (prev_stack - next_stack);
        }
    }
    /* eax ebx ecx edx esi edi */
    for (i = 0; i < 5; i++)
    {
        if (prev_stack <= c->registers[i] && c->registers[i] <= c->stack_bottom)
        {
            c->registers[i] -= (c->registers[i] - next_stack);
        }
    }
    cont_stack_expander((c->stack_size + 1000) / 10);
    memcpy((uint8_t*)next_stack, c->stack, c->stack_size);
    uint32_t diff = c->registers[6] - c->registers[7];
    c->registers[7] = next_stack;
    c->registers[6] = next_stack + diff;
    current_stack_bottom = next_stack + c->stack_size;
    mylongjmp(c->registers, r);
}

int cont_save(Cont* c)
{
    int ret = mysetjmp(c->registers);
    if (ret != 0)
    {
        return ret;
    }

    uint32_t current_stack = c->registers[7];
    c->stack_size = current_stack_bottom - current_stack;
#ifdef USE_BOEHM_GC
    c->stack = (uint8_t*)GC_MALLOC(c->stack_size);
#elif defined(USE_MONA_GC)
    c->stack = (uint8_t*)gc_malloc_has_pointer(c->stack_size);
#else
    c->stack = (uint8_t*)malloc(c->stack_size);
#endif
    c->stack_bottom = current_stack_bottom;
    memcpy(c->stack, (uint8_t*)current_stack, c->stack_size);
    return ret;
}
