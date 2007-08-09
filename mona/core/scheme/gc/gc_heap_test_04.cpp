/*!
    \file   gc_heap_test_04.cpp
    \brief

    Copyright (c) 2002-2007 Higepon.
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/
#include "GCRecord.h"

extern GCRecord root;

static char* dont_sweep;
typedef struct
{
    char* p;
    uint32_t dummy;
} HasHeapRef;

static HasHeapRef* h;

void test()
{
    char* p = new char[11];
    char* q = new char[12];
}

void test2()
{
    char* hoge = new char[15];
}

void test3()
{
    short* r = new short;
    char* t = new char[13];
    for (int i = 0; i < 5; i++)
    {
        test2();
    }

    dont_sweep = new char[14];
    h = new HasHeapRef;
    h->p = new char[16];

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

int main(int argc, char *argv[])
{
    gc_init();

    int x = 0x12345678;
    test();
    test3();
    int y = 0x99998888;
    dummy(0);
    dont_sweep = NULL; // can sweep
    h = NULL;
    gc();

    int size = gc_record_size(&root);

    if (size == 0)
    {
        printf("[OK] %s\n", argv[0]);
        return 0;
    }
    else
    {
        printf("%s [NG]\n", argv[0]);
        FOREACH_GC_RECORD(&root, e)
        {
            printf("    not sweeped size = %d\n", e->size);
        }
        this_variable_is_refered(h);
        return 1;
    }

}
