#ifndef _MONA_THREAD_
#define _MONA_THREAD_

#include "Node.h"
#include "KObject.h"
#include "sys/List.h"
#include "sys/types.h"
#include "sys/MEvent.h"

#define IN_SAME_SPACE(a, b) ((a->archinfo->cr3) == (b->archinfo->cr3))

/*----------------------------------------------------------------------
    Thread
----------------------------------------------------------------------*/
class Thread : public Node, public KObject
{
public:
    Thread();
    virtual ~Thread();

    inline int getType() const
    {
        return THREAD;
    }

public:
    void Tick()
    {
        age++;
        lastCpuUsedTick++;
    }

public:
    int waitEvent;
    dword priority;
    dword basePriority;
    int index; // for debug only
    ThreadInfo* tinfo;
    List<MessageInfo*>* messageList;
    dword id;
    dword lastCpuUsedTick;
    dword age;
    dword schedulerTotalTick;
    dword flags;
    dword wakeupTimer;
};

#endif
