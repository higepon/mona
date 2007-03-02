#include "GCNode.h"

extern GCNode top;

void test()
{
    char* p = new char[32];
    char* q = new char;
}

void test3()
{
    char* hoge = new char[33];
}

void test2()
{
    short* r = new short;
    char* t = new char[5];
    for (int i = 0; i < 5; i++)
    {
        test3();
    }
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
    int* q = new int;
    test();
    test2();
    int y = 0x99998888;
    char* buf = new char[64];
    dummy(0);
    gc();
    int size = gc_node_size(&top);

    if (size == 2 && top.next->size == 64 && top.next->next->size == 4)
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
