#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "types.h"
#include "aq.h"

#define FOREACH_Q(top, type, element) \
for (type element = (type )((top)->next); element != (top); element = (type )((element)->next))

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
    Queue* top = new Queue();
    top->initialize();

    printf("Queue Test\n");

    for (int i = 0; i < 10; i++)
    {
        Queue* q = new Queue();
        printf("[%x]", q);
        top->addToPrev(q);
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
    Array<Queue*> runq(10);
    for (int i = 0; i < 10; i++) runq[i] = new Queue();

    printf("\nArray and Queue Test\n");

    FOREACH(Queue*, value, runq)
    {
        value->initialize();
        Queue* q = new Queue();
        printf("[%x]", q);
        top->addToPrev(q);
    }

    FOREACH(Queue*, value, runq)
    {
        FOREACH_Q(value, Queue*, hoge)
       {
           printf("<%x>", hoge);
       }
    }

    return 0;
}

void Queue::initialize()
{
    this->prev = this;
    this->next = this;
}

void Queue::addToNext(Queue* q)
{
    q->next = this->next;
    q->prev = this;
    this->next->prev = q;
    this->next = q;
}

void Queue::addToPrev(Queue* q)
{
    q->prev = this->prev;
    q->next = this;
    this->prev->next = q;
    this->prev = q;
}

void Queue::remove()
{
    this->prev->next = this->next;
    this->next->prev = this->prev;
}

bool Queue::isEmpty()
{
    return (this->next == this);
}

Queue* Queue::removeNext()
{
    Queue* result = this->next;
    this->next = result->next;
    result->next->prev = this;
    return result;
}

Queue* Queue::top()
{
    return this->next;
}
