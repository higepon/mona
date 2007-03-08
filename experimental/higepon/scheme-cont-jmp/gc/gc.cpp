#include "GCNode.h"
#include "mysetjmp.h"

GCNode top;
GCNode freeNodes;
static char* gc_stack_bottom;
static char* gc_data_start;
static char* gc_data_end;
static uint32_t gc_heap_min;
static uint32_t gc_heap_max;
static uint32_t gc_count;

#ifdef GC_TEST
GCNode test_top;
#endif

static void gc_mark_heap(GCNode* node);
static void gc_mark_registers();
static GCNode* gc_alloc_node();
static GCNode* gc_copy_node(GCNode* n);

static bool gc_initialized = false;

static uint32_t g_8  = 0;
static uint32_t g_16 = 0;
static uint32_t g_32 = 0;
static uint32_t g_64 = 0;
static uint32_t g_128 = 0;
static uint32_t g_256 = 0;
static uint32_t g_512 = 0;
static uint32_t g_1024 = 0;
static uint32_t g_2048 = 0;
static uint32_t g_last = 0;


void* operator new(unsigned int size)
{
    if (!gc_initialized) {gc_init(); gc_initialized = true;}
//    unsigned int allocSize = (size + 15) & 0xfffffff0;

    if (size <= 8) g_8++;
    if (8 < size && size <=16) g_16++;
    if (16 < size && size <=32) g_32++;
    if (32 < size && size <=64) g_64++;
    if (64 < size && size <=128) g_128++;
    if (128 < size && size <=256) g_256++;
    if (256 < size && size <=512) g_512++;
    if (512 < size && size <=1024) g_1024++;
    if (1024 < size && size <=2048) g_2048++;
    if (size > 2048) g_last++;

    void* ret = malloc(size);
    ASSERT_NOT_NULL(ret);
    if ((uint32_t)ret <= GC_SAFE_POINTER(gc_heap_min)) gc_heap_min = GC_SAFE_POINTER(ret - 1);
    if ((uint32_t)ret >= GC_SAFE_POINTER(gc_heap_max)) gc_heap_max = GC_SAFE_POINTER(ret + 1);

    GC_TRACE_OUT("operator new(%d) = %p\n", size, ret);
    GCNode* n = gc_alloc_node();
    n->address = (void*)GC_SAFE_POINTER(ret);
    n->size = size;
    n->haspointer = true;
    gc_node_add_to_next(&top, n);
// #ifdef GC_TEST
//     GCNode* testNode = gc_copy_node(n);
//     gc_node_add_to_next(&test_top, testNode);
// #endif
    gc_count++;
    bool do_gc = gc_count % 100000 == 0;
    if (do_gc) gc();
    return ret;
}

void* operator new(unsigned int size, bool haspointer)
{
    if (!gc_initialized) {gc_init(); gc_initialized = true;}
    if (size <= 8) g_8++;
    if (8 < size && size <=16) g_16++;
    if (16 < size && size <=32) g_32++;
    if (32 < size && size <=64) g_64++;
    if (64 < size && size <=128) g_128++;
    if (128 < size && size <=256) g_256++;
    if (256 < size && size <=512) g_512++;
    if (512 < size && size <=1024) g_1024++;
    if (1024 < size && size <=2048) g_2048++;
    if (size > 2048) g_last++;

//    unsigned int allocSize = (size + 15) & 0xfffffff0;
    void* ret = malloc(size);
    ASSERT_NOT_NULL(ret);
    if ((uint32_t)ret <= GC_SAFE_POINTER(gc_heap_min)) gc_heap_min = GC_SAFE_POINTER(ret - 1);
    if ((uint32_t)ret >= GC_SAFE_POINTER(gc_heap_max)) gc_heap_max = GC_SAFE_POINTER(ret + 1);

    GC_TRACE_OUT("operator new(%d) = %p\n", size, ret);
    GCNode* n = gc_alloc_node();
    n->address = (void*)GC_SAFE_POINTER(ret);
    n->size = size;
    n->haspointer = haspointer;
    gc_node_add_to_next(&top, n);
// #ifdef GC_TEST
//     GCNode* testNode = gc_copy_node(n);
//     gc_node_add_to_next(&test_top, testNode);
// #endif
    gc_count++;
    bool do_gc = gc_count % 100000 == 0;
    if (do_gc) gc();
    return ret;
}

void gc_init_internal(char* stack_bottom, char* data_start, char* data_end)
{
    if (gc_initialized) return;
    gc_initialized = true;
    gc_count = 0;
    gc_heap_max = 0;
    gc_heap_min = GC_SAFE_POINTER(0xfffffff);
    gc_stack_bottom = stack_bottom;
    gc_data_start   = data_start;
    gc_data_end     = data_end;
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
    printf("*********************mark\n");
    register char* stack_top asm ("%esp");
    GC_TRACE_OUT("    ==== mark start ====\n");

    int d = (uint32_t)gc_stack_bottom - (uint32_t)stack_top;
    printf("stack size = %d\n", d);

    uint32_t total = 0;
    FOREACH_GC_NODE(&top, n)
    {
        n->reachable = false;
        total += n->size;
    }


    printf("*********************stack %d\n", total);
    printf("node size = %d\n", gc_node_size(&top));
    for (int i = 0; i < d; i++)
    {
        uint32_t valueOnStack = *((uint32_t *) &stack_top[i]);
        FOREACH_GC_NODE(&top, n)
        {
            //GC_TRACE_OUT("          stack_top[i] = %x\n", valueOnStack);
            //if (stack_top[i] >= 0x8000000 && esp[i] < 0x9000000) GC_TRACE_OUT("[%x]esp[i] = %x\n", &esp[i], esp[i]);
            if (GC_SAFE_POINTER(n->address) == valueOnStack && !n->reachable)
            {
                GC_TRACE_OUT("        [mark stack] %x\n", valueOnStack);
                n->reachable = true;
            }
        }
    }
    printf("*********************stack end\n");
    printf("*********************global\n");
    d = (uint32_t)gc_data_end - (uint32_t)gc_data_start;
    for (int i = 0; i < d; i++)
    {
        uint32_t valueOnData = *((uint32_t *) &gc_data_start[i]);
        FOREACH_GC_NODE(&top, n)
        {
            //GC_TRACE_OUT("          stack_top[i] = %x\n", valueOnStack);
            //if (stack_top[i] >= 0x8000000 && esp[i] < 0x9000000) GC_TRACE_OUT("[%x]esp[i] = %x\n", &esp[i], esp[i]);
            if (GC_SAFE_POINTER(n->address) == valueOnData && !n->reachable)
            {
                GC_TRACE_OUT("        [mark global] %x\n", valueOnData);
                n->reachable = true;
            }
        }
    }
    printf("*********************global end\n");
    printf("*********************heap\n");
    printf("min = %x max = %x\n", GC_SAFE_POINTER(gc_heap_min), GC_SAFE_POINTER(gc_heap_max));
    FOREACH_GC_NODE(&top, n)
    {
        gc_mark_heap(n);
    }
    printf("*********************heap end\n");
    gc_mark_registers();

    GC_TRACE_OUT("    ==== mark end   ====\n");
}

void gc_mark_registers()
{
    printf("*********************register\n");
    myjmp_buf registers;
    mysetjmp(registers);
    for (int i = 0; i < _JBLEN; i++)
    {
        uint32_t valueOnRegister = registers[i];
        FOREACH_GC_NODE(&top, n)
        {
            if (GC_SAFE_POINTER(n->address) == valueOnRegister && !n->reachable)
            {
                GC_TRACE_OUT("        [mark ] %x\n", valueOnRegister);
                n->reachable = true;
            }
        }
    }
}

void gc_mark_heap(GCNode* node)
{
    if (!node->reachable || !node->haspointer) return;
    uint32_t size = node->size;
    char* address = (char*)GC_SAFE_POINTER(node->address);
    for (uint32_t i = 0; i < size; i++)
    {
        uint32_t valueOnHeap = *((uint32_t *) &address[i]);
        if (valueOnHeap > GC_SAFE_POINTER(gc_heap_max) || valueOnHeap < GC_SAFE_POINTER(gc_heap_min)) continue;
        FOREACH_GC_NODE(&top, n)
        {
            if (!n->reachable && GC_SAFE_POINTER(n->address) == valueOnHeap)
            {
                GC_TRACE_OUT("        [mark heap] %x\n", valueOnHeap);
                n->reachable = true;
                gc_mark_heap(n);
            }
        }
    }
}

void gc_sweep()
{
    printf("*********************sweep\n");
    GC_TRACE_OUT("    ==== sweep start ====\n");

    FOREACH_GC_NODE(&top, n)
    {
        if (!n->reachable)
        {
            GCNode* prev = n->prev;
            GC_TRACE_OUT("        [sweep] %p\n",  GC_SAFE_POINTER(n->address));
            free((void*)GC_SAFE_POINTER(n->address));
            gc_node_remove(n);
            gc_node_add_to_next(&freeNodes, n);
            n = prev;
        }
    }
    GC_TRACE_OUT("    ==== sweep end   ====\n");
}

void gc()
{
    printf("*********************gc\n");
    printf("8 %d, 16 %d, 32 %d, 64 %d, 128 %d, 256 %d, 512 %d, 1024 %d, 2048 %d, last %d", g_8, g_16, g_32, g_64, g_128, g_256, g_512, g_1024, g_2048, g_last); 

    gc_mark();
//    gc_sweep();
}

GCNode* gc_alloc_node()
{
    if (gc_node_is_empty(&freeNodes))
    {
        return gc_node_alloc();
    }
    GCNode* ret = gc_node_remove_next(&freeNodes);
    gc_node_initialize(ret);
    return ret;
}

GCNode* gc_copy_node(GCNode* n)
{
    GCNode* ret = gc_alloc_node();
    ret->address = n->address;
    ret->size    = n->size;
    return ret;
}
