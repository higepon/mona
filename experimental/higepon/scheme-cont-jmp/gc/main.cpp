#include "GCNode.h"
#include <stdint.h>


#define GC_TEST
extern GCNode top;
extern GCNode freeNodes;
extern char* ebp;

#ifdef GC_TEST
GCNode test_top;
#endif


void test()
{
    char* p = new char[32];
    int* q = new int;
}

void test2()
{
    new int;
}

int main(int argc, char *argv[])
{
    asm volatile("movl %%ebp, %0" : "=g"(ebp));
    int x = 0x12345678;
#ifdef GC_TEST
    gc_node_initialize(&test_top);
#endif
    gc_node_initialize(&top);
    gc_node_initialize(&freeNodes);

    test();
    test2();
    int y = 0x99998888;
    gc();
    return 0;
}
