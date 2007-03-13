#include "GCNode.h"

extern GCNode top;

int main(int argc, char *argv[])
{
    gc_init();

    for (int i = 0; i < 80000; i++)
    {
        new char [200];
    }
    gc();
}
