/*!
    \file   Condition.h
    \brief  Condition variable.

    Copyright (c) 2002-2009 Higepon and the individuals listed on the ChangeLog entries.
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2009/11/24
*/
#ifndef _MONA_CONDITION_
#define _MONA_CONDITION_

#include "KObject.h"

class Condition : public KObject {
public:
    Condition() : waitList_(new HList<Thread*>) {}
    virtual ~Condition() {}

    intptr_t getType() const
    {
        return CONDITION;
    }

    intptr_t wait(Thread* thread)
    {
        enter_kernel_lock_mode();
        waitList_->add(thread);
        thread->setWaitingCondition(this);
        g_scheduler->WaitEvent(thread, MEvent::CONDITION_NOTIFY);
        return Scheduler::YIELD;
    }

    bool removeFromWaitList(Thread* thread)
    {
        return waitList_->remove(thread);
    }

    intptr_t waitTimeout(Thread* thread, intptr_t timeoutTick)
    {
        enter_kernel_lock_mode();
        waitList_->add(thread);
        thread->setWaitingCondition(this);
        g_scheduler->Sleep(thread, timeoutTick);
        // todo
        g_scheduler->WaitEvent2(thread, MEvent::SLEEP, MEvent::CONDITION_NOTIFY);
        return Scheduler::YIELD;
    }

    intptr_t notifyAll()
    {
        enter_kernel_lock_mode();
        while (!waitList_->isEmpty()) {
            Thread* thread = NULL;
            bool isRemoved = waitList_->removeAt(0, &thread);
            ASSERT(isRemoved);
            thread->setWaitingCondition(NULL);
            g_scheduler->EventComes(thread, MEvent::CONDITION_NOTIFY);
        }
        return Scheduler::YIELD;
    }

private:
    List<Thread*>* waitList_;


};

#endif
