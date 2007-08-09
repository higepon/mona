/*!
    \file   GCRecord.cpp
    \brief

    Copyright (c) 2002-2007 Higepon.
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/
#include "GCRecord.h"

void gc_record_initialize(GCRecord* n)
{
    GC_ASSERT_NOT_NULL(n);
    n->prev = n;
    n->next = n;
    n->magic = GC_MAGIC;
    n->reachable = 0;
    n->age = 0;
}

void gc_record_add_to_next(GCRecord* left, GCRecord* right)
{
    GC_ASSERT_NOT_NULL(left);
    GC_ASSERT_NOT_NULL(right);
    right->next = left->next;
    right->prev = left;
    left->next->prev = right;
    left->next = right;
}

void gc_record_add_to_prev(GCRecord* right, GCRecord* left)
{
    GC_ASSERT_NOT_NULL(left);
    GC_ASSERT_NOT_NULL(right);
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
