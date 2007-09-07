/*!
    \file   gc_global_test_02.cpp
    \brief

    Copyright (c) 2002-2007 Higepon.
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/
#include "gc_helper.h"

static char* dont_sweep;
static char huge[10];

void test()
{
    char* p = new char[32];
    char* q = new char;
}

void test3()
{
    char* hoge = new char[33];
    huge[0] = 'a';
}

void test2()
{
    short* r = new short;
    char* t = new char[5];
    for (int i = 0; i < 5; i++)
    {
        test3();
    }
    dont_sweep = new char[34];
}

int dummy(int i)
{
    if (i > 100) return 100;
    return dummy(i + 1);
}

// this variable is refered, so gcc can't erase variables.
void this_variable_is_refered(void* p)
{
    volatile void* h = p;
    FILE* f = fopen("/dev/null", "w+");
    fprintf(f, "%x", p);
    fclose(f);
}


// sweep OK pattern
int main(int argc, char *argv[])
{
    gc_init();

    int x = 0x12345678;
    int* q = new int;
    test();
    test2();
    int y = 0x99998888;
    dummy(0);
    dont_sweep = NULL; // can sweep
    gc();

    this_variable_is_refered(q);
    int expected[] = {4};
    show_gc_result(expected, sizeof(expected) /sizeof(int), argv[0]);
}
