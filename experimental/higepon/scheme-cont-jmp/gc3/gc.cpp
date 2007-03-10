#include "GCRecord.h"

GCRecord root;
//static GCRecord root;
static uint32_t gc_heap_min;
static uint32_t gc_heap_max;
static char* gc_stack_bottom;
static char* gc_data_start;
static char* gc_data_end;
static bool gc_initialized = false;
static int gc_count = 0;

static bool gc_check_valid_record(GCRecord* r);
static GCRecord* gc_is_memory_block(uint32_t address);
static bool gc_check_valid_record(GCRecord* r);
static void gc_mark_stack();
static void gc_mark_global();
static void gc_mark_heap(GCRecord* r);
static void gc_mark_registers();
static void gc_mark();
static void gc_sweep();

#pragma weak __libc_stack_end
extern void* __libc_stack_end;
void* get_stack_bottom()
{
//     long pagesize = sysconf(_SC_PAGESIZE);
//     return (void*)(((uintptr_t)__libc_stack_end + pagesize) & ~(pagesize - 1));
    GC_TRACE_OUT("__libc_stack_end = %x\n", __libc_stack_end);
    return __libc_stack_end;
}


void* operator new(unsigned int size)
{
    return gc_malloc(size);
}

void* operator new(unsigned int size, bool haspointer)
{
    return gc_malloc(size, haspointer);
}

void* operator new[](size_t size)
{
    return gc_malloc(size);
}

void* operator new[](unsigned int size, bool haspointer)
{
    return gc_malloc(size, haspointer);
}

void gc()
{
    gc_mark();
    gc_sweep();
}

void gc_init_internal(char* stack_bottom, char* data_start, char* data_end)
{
    GC_TRACE_OUT("    ==== %s ==== \n", __func__);
    if (gc_initialized)
    {
        return;
    }
    gc_record_initialize(&root);
    gc_heap_max = GC_SAFE_POINTER(0);
    gc_heap_min = GC_SAFE_POINTER(0xfffffff);
#if 0
    gc_stack_bottom = stack_bottom;
#else
    gc_stack_bottom = (char*)get_stack_bottom();
#endif
    gc_data_start   = data_start;
    gc_data_end     = data_end;
    gc_initialized  = true;
}

void* gc_malloc(uint32_t size, bool haspointer)
{
    if (!gc_initialized)
    {
        gc_init();
    }

    // gc の呼出しは割り当て前に行わないとdouble free の可能性が
    gc_count++;
    if (gc_count % 1000 == 0)
    {
        gc();
    }

    uint32_t alloc_size = sizeof(GCRecord) + size;
    GCRecord* r = (GCRecord*)malloc(alloc_size);
    GC_ASSERT_NOT_NULL(r);
    if ((uint32_t)r < GC_SAFE_POINTER(gc_heap_min)) gc_heap_min = GC_SAFE_POINTER(r);
    if ((uint32_t)r + alloc_size > GC_SAFE_POINTER(gc_heap_max)) gc_heap_max = GC_SAFE_POINTER(r + alloc_size);

    gc_record_initialize(r);
    gc_record_add_to_next(&root, r);
    r->size = size;
    r->haspointer = 1;
    GC_TRACE_OUT("    ==== %x(%d) new ====\n", r->data, size);
    return r->data;
}

void gc_free(GCRecord* r)
{
    r->magic = GC_MAGIC_NOT_USED;
    free(r);
}

GCRecord* gc_is_memory_block(uint32_t address)
{
    if (address < GC_SAFE_POINTER(gc_heap_min) || address > GC_SAFE_POINTER(gc_heap_max)) return NULL;
    GCRecord* r = (GCRecord*)(address - sizeof(GCRecord));
    if (!gc_check_valid_record(r)) return NULL;
    return r;
}

bool gc_check_valid_record(GCRecord* r)
{
    return r->magic == GC_MAGIC;
}

void gc_mark_block(GCRecord* r)
{
    r->reachable = 1;
    return;
}

void gc_mark_range(char* from, char* to)
{
    GC_ASSERT((uint32_t)to > (uint32_t)from);
    uint32_t size = (uint32_t)to - (uint32_t)from;
    for (uint32_t i = 0; i < size; i++)
    {
        uint32_t value = *((uint32_t *)&from[i]);
        GCRecord* r = gc_is_memory_block(value);
        if (NULL != r)
        {
            GC_TRACE_OUT("      mark=%x\n", value);
            gc_mark_block(r);
        }
    }
}

void gc_mark_stack()
{
    GC_TRACE_OUT("    ==== %s start ====\n", __func__);
    register char* stack_top asm ("%esp");
    gc_mark_range(stack_top, gc_stack_bottom);
    GC_TRACE_OUT("    ==== %s end ====\n", __func__);
}

void gc_mark_global()
{
    GC_TRACE_OUT("    ==== %s start ====\n", __func__);
    gc_mark_range(gc_data_start, gc_data_end);
    GC_TRACE_OUT("    ==== %s end ====\n", __func__);
}

void gc_mark_registers()
{
    myjmp_buf registers;
    mysetjmp(registers);
    for (int i = 0; i < _JBLEN; i++)
    {
        uint32_t valueOnRegister = registers[i];
        GCRecord* r = gc_is_memory_block(valueOnRegister);
        if (NULL != r)
        {
            GC_TRACE_OUT("      mark=%x\n", valueOnRegister);
            gc_mark_block(r);
        }
    }
}

void gc_mark_heap(GCRecord* r)
{
    if (!r->reachable || !r->haspointer || r->checkdone) return;
    r->checkdone = 1;
    uint32_t size = r->size;
    char* address = r->data;
    for (uint32_t i = 0; i < size; i++)
    {
        uint32_t valueOnHeap = *((uint32_t *) &address[i]);
//         if (size == 136 && valueOnHeap > 0x8000000 && valueOnHeap < 0x9000000)
//         {
//             printf("valueOnHeap[%d] = %x %d\n", i, valueOnHeap, gc_is_memory_block(valueOnHeap));
//             GCRecord* r = (GCRecord*)(valueOnHeap - sizeof(GCRecord));
//             printf("r->magic=%x\n", r->magic);
//         }

        GCRecord* target = gc_is_memory_block(valueOnHeap);
        if (NULL != target)
        {
            GC_TRACE_OUT("      heap mark=%x\n", valueOnHeap);
            gc_mark_block(target);
            gc_mark_heap(target);
        }
    }
}

void gc_mark()
{
    GC_TRACE_OUT("    ==== %s start ====\n", __func__);
    FOREACH_GC_RECORD(&root, r)
    {
        r->reachable = 0;
        r->checkdone = 0;
    }

    gc_mark_stack();
    gc_mark_global();
    gc_mark_registers();
    FOREACH_GC_RECORD(&root, r)
    {
        gc_mark_heap(r);
    }

    GC_TRACE_OUT("    ==== %s end ====\n", __func__);
}

void gc_sweep()
{
    GC_TRACE_OUT("    ==== %s start ====\n", __func__);
    FOREACH_GC_RECORD(&root, r)
    {
        if (!r->reachable)
        {
            GC_TRACE_OUT("      sweep=%x\n", r->data);
//            printf("      sweep=%x\n", r->data);
            GCRecord* prev = r->prev;
            gc_record_remove(r);
            gc_free(r);
            r = prev;
        }
    }
    GC_TRACE_OUT("    ==== %s end ====\n", __func__);
}
