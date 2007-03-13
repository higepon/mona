#include "GCNode.h"

extern GCNode top;

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
    int size = gc_node_size(&top);

    if (size == 1 && top.next->size == 4)
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
