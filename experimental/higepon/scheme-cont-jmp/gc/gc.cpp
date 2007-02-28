#include "GCNode.h"

GCNode top;
GCNode freeNodes;
char* ebp;

#ifdef GC_TEST
GCNode test_top;
#endif


void* operator new(unsigned int size)
{
    gc();
    void* ret = malloc(size);
    ASSERT_NOT_NULL(ret);
    printf("operator new(%d) = %p\n",
        size, ret);
    GCNode* n = gc_node_alloc();
    n->address = ret;
    n->size = size;
    gc_node_add_to_next(&top, n);
#ifdef GC_TEST
    GCNode* testNode = gc_node_copy(n);
    gc_node_add_to_next(&test_top, testNode);
#endif

    return ret;
}

void gc_mark()
{
    register char* esp asm ("%esp");
    printf("    ==== mark start ====\n");
    static int c = 0;
    c++;

    int d = (uint32_t)ebp - (uint32_t)esp;

    FOREACH_GC_NODE(&top, n)
    {
        n->reachable = false;
    }

    for (int i = 0; i < d; i++)
    {
        uint32_t valueOnStack = *((uint32_t *) &esp[i]);
        FOREACH_GC_NODE(&top, n)
        {
//            printf("[%x]esp[i] = %x\n", &esp[i], valueOnStack);
            //if (esp[i] >= 0x8000000 && esp[i] < 0x9000000) printf("[%x]esp[i] = %x\n", &esp[i], esp[i]);
            if ((uint32_t)n->address == valueOnStack && !n->reachable)
            {
                printf("      [mark reachable%d] %x\n", c, valueOnStack);
                n->reachable = true;
            }
        }
    }
    printf("    ==== mark end   ====\n");
}

void gc_sweep()
{
    printf("    ==== sweep start ====\n");
    static int c = 0;
    c++;

    FOREACH_GC_NODE(&top, n)
    {
        if (!n->reachable)
        {
            GCNode* prev = n->prev;
            printf("      [sweep%d] %p\n", c,  n->address);
            free(n->address);
            gc_node_remove(n);
            gc_node_add_to_next(&freeNodes, n);
            n = prev;
        }
    }
    printf("    ==== sweep end   ====\n");
}

void gc()
{
    gc_mark();
    gc_sweep();
}
