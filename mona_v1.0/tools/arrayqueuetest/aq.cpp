#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "types.h"
#include "aq.h"

#define FOREACH_Q(top, type, element) \
for (type element = (type )((top).next); element != &(top); element = (type )((element)->next))

#define FOREACH(type, iterator, array) \
    if ((array).getLength() > 0) \
        for ({int __i = 0; type iterator;} \
            __i < (array).getLength() && (&(iterator = (array)[__i]) || true); __##i++)

/*
    main()
*/
int main(int argc, char** argv)
{
    /* only Queue test */
    Queue top;
    Queue::initialize(&top);

    printf("Queue Test\n");

    for (int i = 0; i < 10; i++)
    {
        Queue* q = new Queue();
        printf("[%x]", q);
        Queue::addToPrev(&top, q);
    }

    FOREACH_Q(top, Queue*, hoge)
    {
        printf("<%x>", hoge);
    }

    /* only Array test */
    Array<int> array(10);

    printf("\nArray Test\n");

    for (int i = 0; i < array.getLength(); i++)
    {
        array[i] = i;
    }

    FOREACH(int, value, array)
    {
        printf("[%d]", value);
    }

    /* Array  & Queue test */
    Array<Queue> runq(10);

    printf("\nArray and Queue Test\n");

    FOREACH(Queue, value, runq)
    {
        Queue::initialize(&value);
        Queue* q = new Queue();
        printf("[%x]", q);
        Queue::addToPrev(&top, q);
    }

    FOREACH(Queue, value, runq)
    {
//        FOREACH_Q(value, Queue*, hoge)
       for (Queue* hoge = (Queue* )((value).next); hoge != &(value); hoge = (Queue* )((hoge)->next))
       {
           printf("<%x>", hoge);
           fflush(stdout);
       }
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
