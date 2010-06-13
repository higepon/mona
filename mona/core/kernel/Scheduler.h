#ifndef _MONA_SCHEDULER_
#define _MONA_SCHEDULER_

#include "Thread.h"
#include "Array.h"
#include "Process.h"
#include <sys/ThreadPriority.h>
#include <sys/HList.h>
#include "KTimer.h"

/*----------------------------------------------------------------------
    Scheduler
----------------------------------------------------------------------*/
class Scheduler
{
public:
    Scheduler();
    virtual ~Scheduler();

    enum {
        YIELD = 1, // should be positive.
        forbidden_comma
    };

public:
    bool Schedule1();
    bool Schedule2();
    void SwitchToNext();

    void Join(Thread* thread)
    {
        Join(thread, ThreadPriority::Normal);
    }

    void Join(Thread* thread, uint32_t basePriority)
    {
        thread->basePriority = basePriority >= maxPriority ? maxPriority - 1 : basePriority;
        thread->priority     = thread->basePriority;


        ASSERT(thread->priority < maxPriority);

        runq[thread->priority]->AddToPrev(thread);
    }

    void ChangeBasePriority(Thread* thread, uint32_t basePriority)
    {
        ASSERT(thread);

        if(thread->basePriority >= maxPriority)
        {
            thread->basePriority = maxPriority - 1;
            return;
        }

        thread->basePriority = basePriority;
    }

    void Tick()
    {
        totalTick++;
    }

    uint32_t GetTick() const
    {
        return totalTick;
    }

    void Dump();
    void SetDump();
    void Sleep(Thread* thread, uint32_t tick);
    void WaitEvent(Thread* thread, int eventForWait);
    void WaitEvent2(Thread* thread, int eventForWait1, int eventForWait2);
    int EventComes(Thread* thread, int eventForWait);
    int Kill(Thread* thread);
    intptr_t SetTimer(Thread* thread, uint32_t tick);
    intptr_t KillTimer(uint32_t id, Thread* thread);
    bool SetNextThread();
    Process* FindProcess(uint32_t pid);
    Process* FindProcess(const char* name);
    Thread* Find(uint32_t id);
    uint32_t LookupMainThread(const char* name);
    uint32_t LookupMainThread(Process* process);
    uint32_t Lookup(const char* name);
    PsInfo* ReadDump();
    Thread** GetAllThread(uint32_t* threadNum);

    void ReserveStackDump(uint32_t tid) { reservedTid_ = tid; }

protected:
    void WakeupTimer();
    void WakeupSleep();
    bool WakeupSleep(Thread* thread);
    void SetPriority(Thread* thread);
    void SetPriority(Thread* thread, unsigned char rate);

    void MoveToNewPosition(Array<Thread*>& queue, Thread* thread)
    {
        ASSERT(thread->priority < maxPriority);

        thread->Remove();
        queue[thread->priority]->AddToPrev(thread);
    }

    int GetTargetQeueue(int waitReason) const
    {
        return waitReason % 3;
    }

protected:
    PsInfo* dumpCurrent;
    Array<Thread*> runq;
    Array<Thread*> waitq;
    HList<KTimer*> timers;
    uint32_t maxPriority;
    uint32_t totalTick;
private:
    uint32_t reservedTid_;
};

#endif
