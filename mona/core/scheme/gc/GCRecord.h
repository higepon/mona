/*!
    \file  GCRecord.h
    \brief 

    Copyright (c) 2006-2007 Higepon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/

#ifndef __GC_RECORD_H__
#define __GC_RECORD_H__

#include "gc.h"
#include "string.h"

#define FOREACH_GC_RECORD(top, element) \
    for (GCRecord* element = (top)->next; element != (top); element = element->next)

#define GC_MAGIC 0xABCDEFAA
#define GC_MAGIC_NOT_USED 0xBBBBAAAA

void gc_record_initialize(GCRecord* n);
void gc_record_add_to_next(GCRecord* left, GCRecord* right);
void gc_record_add_to_prev(GCRecord* right, GCRecord* left);
void gc_record_remove(GCRecord* self);
bool gc_record_is_empty(GCRecord* self);
int gc_record_size(GCRecord* self);
GCRecord* gc_record_remove_next(GCRecord* self);

#endif // __GC_RECORD_H__
