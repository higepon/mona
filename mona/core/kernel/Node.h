#ifndef _MONA_NODE_
#define _MONA_NODE_

#include "sys/types.h"
#include "kernel.h"

#define FOREACH_N(top, type, element) \
for (type element = (type )((top)->next); element != (top); element = (type )((element)->next))

extern "C" VirtualConsole*g_console;

/*----------------------------------------------------------------------
    Node
----------------------------------------------------------------------*/
class Node
{
public:

    void Initialize()
    {
        this->prev = this;
        this->next = this;
    }

    void AddToNext(Node* q)
    {
        ASSERT(q);

        q->next = this->next;
        q->prev = this;
        this->next->prev = q;
        this->next = q;
    }

    void AddToPrev(Node* q)
    {
        ASSERT(q);

        q->prev = this->prev;
        q->next = this;
        this->prev->next = q;
        this->prev = q;
    }

    void Remove()
    {
        this->prev->next = this->next;
        this->next->prev = this->prev;
    }

    bool IsEmpty()
    {
        return (this->next == this);
    }

    Node* RemoveNext()
    {
        Node* result = this->next;
        this->next = result->next;
        result->next->prev = this;
        return result;
    }

    Node* Top()
    {
        return this->next;
    }

public:
    Node* next;
    Node* prev;
};

#endif
