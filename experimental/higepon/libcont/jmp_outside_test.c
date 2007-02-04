#include <stdio.h>
#include "mysetjmp.h"

static void func1(int counter);
static myjmp_buf env;

void jmp_outside_test()
{
    if (mysetjmp(env) == 0)
    {
        func1(0);
    }
    else
    {
        printf("jmp outside OK\n");
    }
}

void func1(int counter)
{
    if (counter == 5)
    {
        mylongjmp(env, 1);
    }
    printf("func1 : %d\n", counter);
    func1(counter + 1);
}


