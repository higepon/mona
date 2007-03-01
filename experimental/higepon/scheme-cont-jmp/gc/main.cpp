#include "GCNode.h"
#include <stdint.h>



extern GCNode top;
extern GCNode freeNodes;
#ifdef GC_TEST
extern GCNode test_top;
#endif



void test()
{
    char* p = new char[32];
    char* q = new char;
}

void test2()
{
    short* r = new short;
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
    dummy(0);
    gc();
    return 0;
}
