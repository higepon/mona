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

Array<Thread*> runq(64);
Array<Thread*> waitq(3);

int main(int argc, char** argv)
{
    for (int i = 0; i < runq.getLength(); i++)
    {
        runq[i] = new Thread("r:root");
        runq[i]->initialize();
    }

    for (int i = 0; i < waitq.getLength(); i++)
    {
        waitq[i] = new Thread("w:root");
        waitq[i]->initialize();
    }

    Thread* idle = new Thread("IDLE");
    Thread* init = new Thread("INIT");

    runq[0]->addToPrev(idle);
    runq[0]->addToPrev(init);

    FOREACH_N(runq[0], Thread*, element)
    {
        printf("%s\n", element->name);

    }

    init->remove();

    FOREACH_N(runq[0], Thread*, element)
    {
        printf("%s\n", element->name);

    }

    printf("top=%s\n", ((Thread*)(runq[0]->top()))->name);

    idle->remove();

    printf("%s\n", runq[0]->isEmpty() ? "empty" : "nnn");

    runq[0]->addToPrev(idle);
    runq[0]->addToPrev(init);

    FOREACH(Thread*, queue, runq)
    {
        if (queue->isEmpty())
        {
            continue;
        }
        else
        {
            printf("toptop=%s\n", ((Thread*)(queue->top()))->name);
            break;
        }
    }

    Thread* target = init;

    target->remove();
    waitq[0]->addToPrev(target);

    FOREACH_N(runq[0], Thread*, element)
    {
        printf("r:%s\n", element->name);

    }

    FOREACH_N(waitq[0], Thread*, element)
    {
        printf("w:%s\n", element->name);

    }





    return 0;
}

Thread::Thread(const char* s)
{
    strcpy(name, s);
}

Thread::~Thread()
{
}

/*----------------------------------------------------------------------
    Node
----------------------------------------------------------------------*/
void Node::initialize()
{
    this->prev = this;
    this->next = this;
}

void Node::addToNext(Node* q)
{
    q->next = this->next;
    q->prev = this;
    this->next->prev = q;
    this->next = q;
}

void Node::addToPrev(Node* q)
{
    q->prev = this->prev;
    q->next = this;
    this->prev->next = q;
    this->prev = q;
}

void Node::remove()
{
    this->prev->next = this->next;
    this->next->prev = this->prev;
}

bool Node::isEmpty()
{
    return (this->next == this);
}

Node* Node::removeNext()
{
    Node* result = this->next;
    this->next = result->next;
    result->next->prev = this;
    return result;
}

Node* Node::top()
{
    return this->next;
}
