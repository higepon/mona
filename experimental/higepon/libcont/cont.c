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
    \file   cont.cpp
    \brief  mona kernel start at this point

    mona kernel start at this point.
    before startKernel, os entered protected mode.

    Copyright (c) 2002,2003, 2004 Higepon and the individuals listed on the ChangeLog entries.
    All rights reserved.
    License=MIT/X License

    \author  HigePon
    \version $Revision: 3946 $
    \date   create:2002/07/21 update:$Date: 2007-01-16 21:48:57 +0900 (火, 16  1月 2007) $
*/

#include "cont.h"

static uint32_t cont_stack_bottom;

void* cont_get_stack_pointer()
{
    register void* stack_pointer asm ("%esp");
    return (void*)((unsigned int)stack_pointer + 8);
}

void cont_initialize()
{
    // fix me!
    cont_stack_bottom = (uint32_t)cont_get_stack_pointer() + 50;
}

void cont_destroy(Cont* c)
{
    if (c->stack != NULL)
    {
        free(c->stack);
        c->stack = NULL;
    }
}

void cont_restore(Cont* c)
{
    uint32_t prev_stack = c->registers[7];
    uint32_t next_stack= prev_stack - 1000;
    for (uint32_t i = 0; i < c->stack_size / 4;  i++)
    {
        uint32_t* p = (uint32_t*)c->stack;
        if (prev_stack <= p[i] && p[i] <= cont_stack_bottom)
        {
            p[i] -= (prev_stack - next_stack);
        }
    }

    // eax ebx ecx edx esi edi
    for (int i = 0; i < 5; i++)
    {
        if (prev_stack <= c->registers[i] && c->registers[i] <= cont_stack_bottom)
        {
            c->registers[i] -= (c->registers[i] - next_stack);
        }
    }
    memcpy((uint8_t*)next_stack, c->stack, c->stack_size);
    uint32_t diff = c->registers[6] - c->registers[7];
    c->registers[7] = next_stack;
    c->registers[6] = next_stack + diff;
    mylongjmp(c->registers, 6);
}

int cont_save(Cont* c)
{
    int ret = mysetjmp(c->registers);
    if (ret != 0) return ret;
    uint32_t current_stack = c->registers[7];
    c->stack_size = cont_stack_bottom - current_stack;
    c->stack = (uint8_t*)malloc(c->stack_size);
    memcpy(c->stack, (uint8_t*)current_stack, c->stack_size);
    return ret;
}
