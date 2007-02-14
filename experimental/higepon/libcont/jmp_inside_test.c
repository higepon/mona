#include "cont.h"

static int func1(int counter);
static void stack_destroy(int counter);

static Cont c;
extern uint32_t cont_stack_bottom;
void jmp_inside_test()
{
    int ret = func1(0);
    if (ret != 0)
    {
        printf("jmp inside OK\n");
        return;
    }
    stack_destroy(0);
    cont_restore(&c, 1);
}

int func1(int counter)
{
    if (counter == 20)
    {
        int ret = cont_save(&c);
        if (ret == 0)
        {
            return ret;
        }
        else
        {
            printf("jmp inside OK [%d]\n", counter);
            return ret;
        }
    }
    printf("func1 : %d\n", counter);
    int ret = func1(counter + 1);
    printf("func1 : %d\n", counter);
    return ret;
}

void stack_destroy(int counter)
{
    if (counter == 500)
    {
        return;
    }
    stack_destroy(counter + 1);
}
