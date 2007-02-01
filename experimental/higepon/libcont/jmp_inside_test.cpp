#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mysetjmp.h"

static void func1(int counter);
static void stack_destroy(int counter);
static myjmp_buf env;

extern uint32_t stack_bottom;
uint32_t my_stack_size;
uint8_t* my_stack;
uint32_t stack;
static int retval = 1;
void jmp_inside_test()
{
    func1(0);
    stack_destroy(0);
    memcpy((uint8_t*)env[7], my_stack, my_stack_size);
//     memcpy((uint8_t*)(stack_bottom + 1024), my_stack, my_stack_size);
//     env[6] = stack_bottom + 1024;
//     env[7] = stack_bottom + 1000;
    mylongjmp(env, retval);
}

void func1(int counter)
{
    if (counter == 5)
    {
        if (mysetjmp(env) == 0)
        {
            stack = env[7];
            my_stack_size = stack_bottom - stack;
            printf("%d %d\n", my_stack_size, env[6] - env[7]);
            my_stack = (uint8_t*)malloc(my_stack_size);
            memcpy(my_stack, (uint8_t*)stack, my_stack_size);
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
            return;
        }
        else
        {
            printf("jmp inside OK[%d]\n", counter);
            return;
        }
    }
    printf("func1 : %d\n", counter);
    func1(counter + 1);
}

void stack_destroy(int counter)
{
    if (counter == 500)
    {
        return;
    }
    stack_destroy(counter + 1);
}
