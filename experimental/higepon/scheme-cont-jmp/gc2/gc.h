#ifndef __GC_H__
#define __GC_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define ASSERT_NOT_NULL(p) {if (NULL == p) {printf("ASSERT_NOT_NULL %s:%d: %s\n", __FILE__, __LINE__, #p);}}
#ifdef GC_TRACE
#define GC_TRACE_OUT(...) printf(__VA_ARGS__);
#else
#define GC_TRACE_OUT(...) //
#endif

#define FOREACH_GC_RECORD(top, element) \
    for (GCRecord* element = (top)->next; element != (top); element = element->next)

typedef struct GCRecord
{
    uint32_t index;
    uint32_t size;
    uint32_t unit_size;
    uint32_t alloced_count;
    GCRecord* next;
    GCRecord* prev;
    uint8_t* markbits[0];
} GCRecord;

GCRecord* gc_alloc_reacord(uint32_t alloc_unit_size);
bool gc_is_record_full(GCRecord* r);
void* gc_alloc_from_record(GCRecord* r);

void gc_init();
void* gc_alloc(uint32_t size);
void gc_free(void* address);
uint32_t gc_index(uint32_t size);
void gc_record_add_to_next(GCRecord* left, GCRecord* right);
void gc_record_add_to_prev(GCRecord* right, GCRecord* left);
void gc_record_remove(GCRecord* self);
bool gc_record_is_empty(GCRecord* self);
GCRecord* gc_record_remove_next(GCRecord* self);
int gc_record_size(GCRecord* self);


#endif // __GC_H__
