#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "types.h"
#include "aq.h"

/*
    main()
*/
int main(int argc, char** argv)
{
    Array<int> array(10);

    for (int i = 0; i < array.getLength(); i++)
    {
        array[i] = i;
    }

    for (int i = 0; i < array.getLength(); i++)
    {
        printf("[%d]", array[i]);
    }
    return 0;
}

void Queue::initialize(Queue* queue)
{
    queue->prev = queue;
    queue->next = queue;
}

void Queue::addToNext(Queue* p, Queue* q)
{
    q->next = p->next;
    q->prev = p;
    p->next->prev = q;
    p->next = q;
}

void Queue::addToPrev(Queue* p, Queue* q)
{
    q->prev = p->prev;
    q->next = p;
    p->prev->next = q;
    p->prev = q;
}

void Queue::remove(Queue* p)
{
    p->prev->next = p->next;
    p->next->prev = p->prev;
}

bool Queue::isEmpty(Queue* p)
{
    return (p->next == p);
}

Queue* Queue::removeNext(Queue* p)
{
    Queue* result = p->next;
    p->next = result->next;
    result->next->prev = p;
    return result;
}

Queue* Queue::top(Queue* root)
{
    return root->next;
}
