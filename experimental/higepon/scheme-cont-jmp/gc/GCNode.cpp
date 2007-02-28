#include "GCNode.h"

GCNode* gc_node_copy(GCNode* n)
{
    GCNode* ret = gc_node_alloc();
    ret->address = n->address;
    ret->size    = n->size;
    return ret;
}

GCNode* gc_node_alloc()
{
    GCNode* result = (GCNode*)malloc(sizeof(GCNode));
    ASSERT_NOT_NULL(result);
    gc_node_initialize(result);
    return result;
}

void gc_node_initialize(GCNode* n)
{
    ASSERT_NOT_NULL(n);
    n->prev = n;
    n->next = n;
    n->reachable = false;
    n->address = NULL;
}

void gc_node_add_to_next(GCNode* left, GCNode* right)
{
    ASSERT_NOT_NULL(left);
    ASSERT_NOT_NULL(right);
    right->next = left->next;
    right->prev = left;
    left->next->prev = right;
    left->next = right;
}

void gc_node_add_to_prev(GCNode* right, GCNode* left)
{
    ASSERT_NOT_NULL(left);
    ASSERT_NOT_NULL(right);
    left->prev = right->prev;
    left->next = right;
    right->prev->next = left;
    right->prev = left;
}

void gc_node_remove(GCNode* self)
{
    self->prev->next = self->next;
    self->next->prev = self->prev;
}

bool gc_node_is_empty(GCNode* self)
{
    return (self->next == self);
}

GCNode* gc_node_remove_next(GCNode* self)
{
    GCNode* result = self->next;
    self->next = result->next;
    result->next->prev = self;
    return result;
}
