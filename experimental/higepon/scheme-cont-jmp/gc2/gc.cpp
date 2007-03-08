#include "gc.h"

#define GC_ROOTS_NUM 10

static GCRecord roots[GC_ROOTS_NUM];

void gc_init()
{
    for (int i = 0; i < GC_ROOTS_NUM; i++)
    {
        GCRecord* r = &roots[i];
        r->next = r;
        r->prev = r;
    }
}

uint32_t gc_record_alloc_num(GCRecord* r)
{
    return (8 * (r->size - 1 - sizeof(GCRecord)) - 7) / (1 + 8 * r->unit_size);
}

uint32_t gc_index(uint32_t size)
{
    switch(size)
    {
    case 1 ... 8:
        return 0;
    case 9 ... 16:
        return 1;
    case 17 ... 32:
        return 2;
    case 33 ... 64:
        return 3;
    case 65 ... 128:
        return 4;
    case 129 ... 256:
        return 5;
    case 257 ... 512:
        return 6;
    case 513 ... 1024:
        return 7;
    case 1025 ... 2048:
        return 8;
    default:
        return 9;
    }
}

void* gc_alloc(uint32_t size)
{
    GCRecord* root = &roots[gc_index(size)];
    FOREACH_GC_RECORD(root, r)
    {
        void* address = gc_alloc_from_record(r);
        if (NULL != address) return address;
    }
    GCRecord* r = gc_alloc_reacord(size);
    printf("hoge=%d\n", gc_record_alloc_num(r));
    gc_record_add_to_next(root, r);
    return gc_alloc_from_record(r);
}

void gc_free(void* address)
{

}

GCRecord* gc_alloc_reacord(uint32_t alloc_unit_size)
{
    uint32_t address = (uint32_t)malloc(8192);
    ASSERT_NOT_NULL((uint8_t)address);
    address = (address + 4095) & 0xFFFFF000;
    GCRecord* record = (GCRecord*)address;
    record->alloced_count = 0;
    record->size = 4096;
    record->unit_size = alloc_unit_size;
    record->next = record;
    record->prev = record;
    return record;
}

bool gc_is_record_full(GCRecord* r)
{
    return ((int)(r->size - sizeof(GCRecord) - ((r->alloced_count + 1) * r->unit_size))) < 0;
}

void* gc_alloc_from_record(GCRecord* r)
{
    if (gc_is_record_full(r)) return NULL;
    r->alloced_count++;
    return (void*)((uint32_t)r + sizeof(GCRecord) + r->unit_size * r->alloced_count);
}

void gc_record_add_to_next(GCRecord* left, GCRecord* right)
{
    ASSERT_NOT_NULL(left);
    ASSERT_NOT_NULL(right);
    right->next = left->next;
    right->prev = left;
    left->next->prev = right;
    left->next = right;
}

void gc_record_add_to_prev(GCRecord* right, GCRecord* left)
{
    ASSERT_NOT_NULL(left);
    ASSERT_NOT_NULL(right);
    left->prev = right->prev;
    left->next = right;
    right->prev->next = left;
    right->prev = left;
}

void gc_record_remove(GCRecord* self)
{
    self->prev->next = self->next;
    self->next->prev = self->prev;
}

bool gc_record_is_empty(GCRecord* self)
{
    return (self->next == self);
}

GCRecord* gc_record_remove_next(GCRecord* self)
{
    GCRecord* result = self->next;
    self->next = result->next;
    result->next->prev = self;
    return result;
}

int gc_record_size(GCRecord* self)
{
    int size = 0;
    FOREACH_GC_RECORD(self, element)
    {
        size++;
    }
    return size;
}
