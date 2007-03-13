#include "GCNode.h"

extern GCNode top;

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
    int size = gc_node_size(&top);

    if (size == 0)
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
