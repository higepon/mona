#include "cont.h"

static uint32_t cont_stack_bottom;

void* cont_get_stack_pointer()
{
    register void* stack_pointer asm ("%esp");
    return (void*)((unsigned int)stack_pointer + 8);
}

void cont_initialize()
{
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

    // eax ebx ecx edx esi ediも書き換え
    for (int i = 0; i < 5; i++)
    {
        if (prev_stack <= c->registers[i] && c->registers[i] <= cont_stack_bottom)
        {
            c->registers[i] -= (c->registers[i] - next_stack);
        }
    }



//    exit(-1);
    memcpy((uint8_t*)next_stack, c->stack, c->stack_size);


    uint32_t diff = c->registers[6] - c->registers[7];
    c->registers[7] = next_stack;
    c->registers[6] = next_stack + diff;
//    dummy();
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
