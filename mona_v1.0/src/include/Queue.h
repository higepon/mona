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

class Queue {

  public:
    virtual Queue* getNext() = 0;
    virtual Queue* getPrevious() = 0;
    virtual void setNext(Queue* next) = 0;
    virtual void setPrevious(Queue* previous) = 0;
};

class QueueManager {

  public:
    static void init(Queue* queue);
    static void addToPrevious(Queue* queue, Queue* that);
    static void remove(Queue* that);
    static bool isEmpty(Queue* queue);
    static Queue* getNext(Queue* queue);
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

#endif
