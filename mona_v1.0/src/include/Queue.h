/*!
    \file  Queue.h
    \brief Queue & utilities

    class Queue & QueueMangager

    Copyright (c) 2003 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2003/07/28 update:$Date$
*/
#ifndef _MONA_QUEUE_
#define _MONA_QUEUE_

#include <GraphicalConsole.h>
extern VirtualConsole* g_console;

class Queue {

  public:
    inline virtual Queue* getNext() {return next_;}
    inline virtual Queue* getPrevious() {return previous_;}
    inline virtual void setNext(Queue* next) {next_ = next;}
    inline virtual void setPrevious(Queue* previous) {previous_ = previous;}

  protected:
    Queue* next_;
    Queue* previous_;

};

class QueueManager {

  public:
    static void init(Queue* queue);
    static void addToPrevious(Queue* queue, Queue* that);
    static void remove(Queue* that);
    static bool isEmpty(Queue* queue);
    static Queue* getNext(Queue* queue);
    static Queue* iterate(Queue* queue);
};

inline void QueueManager::init(Queue* queue) {

    queue->setNext(queue);
    queue->setPrevious(queue);
    return;
}

inline void QueueManager::addToPrevious(Queue* queue, Queue* that) {

    that->setPrevious(queue->getNext());
    that->setNext(queue);

    queue->getPrevious()->setNext(that);
    queue->setPrevious(that);
    return;
}

inline void QueueManager::remove(Queue* that) {

    that->getPrevious()->setNext(that->getNext());
    that->getNext()->setPrevious(that->getPrevious());
    return;
}

inline bool QueueManager::isEmpty(Queue* queue) {

    return (queue->getNext() == queue);
}

inline Queue* QueueManager::getNext(Queue* queue) {


    Queue* result = queue->getNext();

    queue->setNext(queue->getNext());
    queue->getNext()->setPrevious(queue);

    return result;
}

inline Queue* QueueManager::iterate(Queue* queue) {

    static Queue* current = NULL;

    if (current == NULL) current = queue;

    current = current->getNext();

    /* end of queue */
    if (current == queue) {

        current = NULL;
    }

    return current;
}

#endif
