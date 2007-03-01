#include "GCNode.h"

GCNode top;
GCNode freeNodes;
static char* gc_stack_bottom;

#ifdef GC_TEST
GCNode test_top;
#endif


void* operator new(unsigned int size)
{
    gc();
    void* ret = malloc(size);
    ASSERT_NOT_NULL(ret);
    GC_TRACE_OUT("operator new(%d) = %p\n",
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

void gc_init_internal(char* stack_bottom)
{
    gc_stack_bottom = stack_bottom;
    gc_node_initialize(&top);
    gc_node_initialize(&freeNodes);
#ifdef GC_TEST
    gc_node_initialize(&test_top);
#endif

    // get caller's gc_stack_bottom, caller may be main().
//    asm volatile("push (%%esp) \n pop %0" : "=g"(gc_stack_bottom));


//    asm volatile("movl (%%esp), %%eax\n movl %%eax,  %0" : "=g"(gc_stack_bottom));

}

void gc_mark()
{
    register char* stack_top asm ("%esp");
    GC_TRACE_OUT("    ==== mark start ====\n");

    int d = (uint32_t)gc_stack_bottom - (uint32_t)stack_top;

    FOREACH_GC_NODE(&top, n)
    {
        n->reachable = false;
    }

    for (int i = 0; i < d; i++)
    {
        uint32_t valueOnStack = *((uint32_t *) &stack_top[i]);
        FOREACH_GC_NODE(&top, n)
        {
            //GC_TRACE_OUT("          stack_top[i] = %x\n", valueOnStack);
            //if (stack_top[i] >= 0x8000000 && esp[i] < 0x9000000) GC_TRACE_OUT("[%x]esp[i] = %x\n", &esp[i], esp[i]);
            if ((uint32_t)n->address == valueOnStack && !n->reachable)
            {
                GC_TRACE_OUT("        [mark ] %x\n", valueOnStack);
                n->reachable = true;
            }
        }
    }
    GC_TRACE_OUT("    ==== mark end   ====\n");
}

void gc_sweep()
{
    GC_TRACE_OUT("    ==== sweep start ====\n");

    FOREACH_GC_NODE(&top, n)
    {
        if (!n->reachable)
        {
            GCNode* prev = n->prev;
            GC_TRACE_OUT("        [sweep] %p\n",  n->address);
            free(n->address);
            gc_node_remove(n);
            gc_node_add_to_next(&freeNodes, n);
            n = prev;
        }
    }
    GC_TRACE_OUT("    ==== sweep end   ====\n");
}

void gc()
{
    gc_mark();
    gc_sweep();
}
