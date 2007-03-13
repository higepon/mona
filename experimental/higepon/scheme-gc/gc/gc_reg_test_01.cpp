#include "GCNode.h"

extern GCNode top;

static char* dont_sweep;

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
}


int dummy(int i)
{
    if (i > 100) return 100;
    return dummy(i + 1);
}

typedef struct
{
    char* p;
    uint32_t dummy;
} HasHeapRef;

typedef struct
{
    HasHeapRef* heapref;
    char dummy[21];
} HasNestHeapRef;

static HasNestHeapRef* h;

int main(int argc, char *argv[])
{
    gc_init();

    int x = 0x12345678;
    h = new HasNestHeapRef;
    h->heapref = new HasHeapRef;
    h->heapref->p = new char[16];
    asm volatile("movl %%eax, %0" : : "m"(h->heapref->p));
    test();
    test3();
    int y = 0x99998888;
    dummy(0);
    dont_sweep = NULL; // can sweep
    gc();
    int size = gc_node_size(&top);

    if (size == 2
        && top.next->size == 8
        && top.next->next->size == 28)
    {
        printf("[OK] %s\n", argv[0]);
        return 0;
    }
    else
    {
        printf("%s [NG]\n", argv[0]);
        FOREACH_GC_NODE(&top, e)
        {
            printf("    not sweeped size = %d\n", e->size);
        }
        return 1;
    }

}
