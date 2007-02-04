#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mysetjmp.h"

static int func1(int counter);
static void stack_destroy(int counter);
static myjmp_buf env;

extern uint32_t stack_bottom;
//uint32_t my_stack_size;
//uint8_t* my_stack;
//uint32_t stack;
//static int retval = 1;

extern void* get_stack_pointer();

#define OK 0

typedef struct Cont
{
    uint32_t stack_size;
    uint8_t* stack;
    myjmp_buf registers;
};

static Cont c;

int dummy()
{
    char buf[1024];
    memset(buf, 1, 1024);
    return buf[1023]; 
}

void cont_restore_stack(Cont* c)
{
    printf("%s start\n", __func__);fflush(stdout);
    uint32_t now_stack = c->registers[7];//(uint32_t)get_stack_pointer();
    uint32_t next_stack= now_stack -  1000;
    printf("now_stack = %x stack_bottom = %x\n", now_stack, stack_bottom);
    for (uint32_t i = 0; i < c->stack_size / 4;  i++)
    {
        uint32_t* p = (uint32_t*)c->stack;
        if (now_stack <= p[i] && p[i] <= stack_bottom)
        {
            printf("%x to ", p[i]);
            p[i] -= (c->registers[7] - next_stack);
            printf("%x\n", p[i]);
        }
    }

    for (int i = 0; i < 5; i++)
    {
        if (now_stack <= c->registers[i] && c->registers[i] <= stack_bottom)
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

void jmp_inside_test()
{
    printf("%s start\n", __func__);fflush(stdout);
    int ret = func1(0);
    if (ret != 0)
    {
        printf("ret =%x\n", ret); fflush(stdout);
        return;
    }
    stack_destroy(0);
#if OK
    memcpy((uint8_t*)c.registers[7], c.stack, c.stack_size);
    mylongjmp(c.registers, 6);
#else
    cont_restore_stack(&c);
#endif
//    cont_restore_stack(&c);
//     memcpy((uint8_t*)(stack_bottom + 1024), my_stack, my_stack_size);
//     env[6] = stack_bottom + 1024;
//     env[7] = stack_bottom + 1000;
//    int a = 6;

}

int func1(int counter)
{
    printf("%s start\n", __func__);fflush(stdout);
    if (counter == 5)
    {
        int ret = mysetjmp(c.registers);

        printf("ret = %d\n", ret);
        if (ret == 0)
        {
#if OK
            uint32_t current_stack = c.registers[7];
            c.stack_size = stack_bottom - c.registers[7];
            printf("%d %d\n", c.stack_size, c.registers[6] - c.registers[7]);
            c.stack = (uint8_t*)malloc(c.stack_size);
            memcpy(c.stack, (uint8_t*)current_stack, c.stack_size);
#else
            uint32_t current_stack = c.registers[7];
            printf("c.registers[7] esp=%x\n", c.registers[7]);
            c.stack_size = stack_bottom - c.registers[7];
            printf("%d %d\n", c.stack_size, c.registers[6] - c.registers[7]);
            c.stack = (uint8_t*)malloc(c.stack_size);
            memcpy(c.stack, (uint8_t*)current_stack, c.stack_size);
            printf("current_stack esp=%x\n", current_stack);
    uint32_t* p = (uint32_t*)current_stack;
    for (int i = 0; i < 10; i++)
    {
        printf("moge %x\n", p[i]);
    }

#endif
//             int size = my_stack_size / 4;
//             uint32_t* p = (uint32_t*)my_stack;
//             for (int i = 0; i < size; i++)
//             {
//                 if (stack <= *(&p[i]) && *(&p[i]) <= stack_bottom)
//                 {
//                     printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
//                     *(&p[i]) = 1024 + *(&p[i]);
//                 }
//             }
            return ret;
        }
        else
        {
            printf("jmp inside OK[%d]\n", counter);
            return ret;
        }
    }
    printf("func1 : %x\n", counter);
    return func1(counter + 1);
//    printf("func1 : %x return \n", counter);
//    return;
}

void stack_destroy(int counter)
{
    if (counter == 500)
    {
        return;
    }
    stack_destroy(counter + 1);
}
