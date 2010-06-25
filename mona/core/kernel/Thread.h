#ifndef _MONA_THREAD_
#define _MONA_THREAD_

#include "Node.h"
#include "KObject.h"
#include "sys/List.h"
#include "sys/types.h"
#include "sys/MEvent.h"
#include "sys/error.h"
#include "Segments.h"
#include "Mutex.h"

#define IN_SAME_SPACE(a, b) ((a->archinfo->cr3) == (b->archinfo->cr3))
#define MULTIPLE_EVENT_MAX 2
/*----------------------------------------------------------------------
    Thread
----------------------------------------------------------------------*/
class KMutex;
class Condition;
class Thread : public Node
{
public:
    Thread();
    virtual ~Thread();

    int isWaiting(int event) const
    {
        for (int i = 0; i < MULTIPLE_EVENT_MAX; i++) {
            if (eventsWaiting[i] == event) {
                return i;
            }
        }
        return -1;
    }

    void clearEventWaiting()
    {
        for (int i = 0; i < MULTIPLE_EVENT_MAX; i++) {
            eventsWaiting[i] = MEvent::NONE;
        }
    }

    void setReturnValue(intptr_t value);

    void setWaitingMutex(KMutex* mutex) {
        ASSERT((mutex != NULL && waitingMutex_ == NULL) ||
               (mutex == NULL && waitingMutex_ == NULL) ||
               (mutex == NULL && waitingMutex_ != NULL));
        waitingMutex_ = mutex;
    }

    void setWaitingCondition(Condition* condition) {
        ASSERT((condition != NULL && waitingCondition_ == NULL) ||
               (condition == NULL && waitingCondition_ == NULL) ||
               (condition == NULL && waitingCondition_ != NULL));
        waitingCondition_ = condition;
    }

    KMutex* getWaitingMutex() const
    {
        return waitingMutex_;
    }

    Condition* getWaitingCondition() const
    {
        return waitingCondition_;
    }

public:
    void Tick()
    {
        age++;
        lastCpuUsedTick++;
    }

public:
    int eventsWaiting[MULTIPLE_EVENT_MAX];
    uint32_t priority;
    uint32_t basePriority;
    int index; // for debug only
    ThreadInfo* tinfo;
    List<MessageInfo*>* messageList;
    uint32_t id;
    uint32_t lastCpuUsedTick;
    uint32_t age;
    uint32_t schedulerTotalTick;
    uint32_t flags;
    uint32_t wakeupSleep;
    class StackSegment* stackSegment;
    //add by TAKA
    uint32_t kernelStackBottom;
private:
    // Waiting mutex.
    // If you want to add another waiting lock primitive,
    // Make this waitingMutex_ to a pointer to lockPrimitive struct.
    KMutex* waitingMutex_;
    Condition* waitingCondition_;
};

#endif
