#ifndef __GC_NODE_H__
#define __GC_NODE_H__

#include "gc.h"

#define FOREACH_GC_NODE(top, element) \
    for (GCNode* element = (top)->next; element != (top); element = element->next)

GCNode* gc_node_copy(GCNode* n);
GCNode* gc_node_alloc();
void gc_node_initialize(GCNode* n);
void gc_node_add_to_next(GCNode* left, GCNode* right);
void gc_node_add_to_prev(GCNode* right, GCNode* left);
void gc_node_remove(GCNode* self);
bool gc_node_is_empty(GCNode* self);
int gc_node_size(GCNode* self);
GCNode* gc_node_remove_next(GCNode* self);

#endif // __GC_NODE_H__
