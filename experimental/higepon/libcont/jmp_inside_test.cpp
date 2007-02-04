#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mysetjmp.h"
#include "cont.h"

static int func1(int counter);
static void stack_destroy(int counter);

#define OK 0
static Cont c;

void jmp_inside_test()
{
    int ret = func1(0);
    if (ret != 0)
    {
        printf("jmp inside OK\n");
        return;
    }
    stack_destroy(0);
#if OK
    memcpy((uint8_t*)c.registers[7], c.stack, c.stack_size);
    mylongjmp(c.registers, 6);
#else
    cont_restore(&c);
#endif
}

int func1(int counter)
{
    if (counter == 5)
    {
#if OK
        int ret = mysetjmp(c.registers);
#else
        int ret = cont_save(&c);
#endif
        if (ret == 0)
        {
#if OK
            uint32_t current_stack = c.registers[7];
            c.stack_size = stack_bottom - c.registers[7];
            printf("%d %d\n", c.stack_size, c.registers[6] - c.registers[7]);
            c.stack = (uint8_t*)malloc(c.stack_size);
            memcpy(c.stack, (uint8_t*)current_stack, c.stack_size);
#else
#endif
            return ret;
        }
        else
        {
            printf("jmp inside OK [%d]\n", counter);
            return ret;
        }
    }
    printf("func1 : %x\n", counter);
    return func1(counter + 1);
}

void stack_destroy(int counter)
{
    if (counter == 500)
    {
        return;
    }
    stack_destroy(counter + 1);
}
