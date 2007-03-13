#include "gc.h"

int main(int argc, char *argv[])
{
    gc_init();
    for (int i = 0; i < 1024; i++)
    {
        void* address = gc_alloc(8);
        printf(" address = %x %d\n", address, i);
        if (NULL == address) break;
    }
}
