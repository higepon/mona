#ifndef _MONA_SCHEDULER_
#define _MONA_SCHEDULER_

#include "Thread.h"
#include "Array.h"
#include "Process.h"

/*----------------------------------------------------------------------
    Scheduler
----------------------------------------------------------------------*/
class Scheduler
{
public:
    Scheduler();
    virtual ~Scheduler();

public:
    bool Schedule1();
    bool Schedule2();

    void Join(Thread* thread)
    {
        ASSERT(thread->priority < maxPriority);

        runq[thread->priority]->AddToPrev(thread);
    }

    void Tick()
    {
        totalTick++;
    }

    dword GetTick() const
    {
        return totalTick;
    }

    void Dump();
    void SetDump();
    void Sleep(Thread* thread, dword tick);
    void WaitEvent(Thread* thread, int waitEvent);
    int EventComes(Thread* thread, int waitEvent);
    int Kill(Thread* thread);
    bool SetNextThread();
    Process* FindProcess(dword pid);
    Process* FindProcess(const char* name);
    Thread* Find(dword id);
    dword LookupMainThread(const char* name);
    dword Lookup(const char* name);
    PsInfo* ReadDump();
    dword* GetAllThreadID(dword* threadNum);

protected:
    void WakeupTimer();
    bool WakeupTimer(Thread* thread);
    void SetPriority(Thread* thread);
    void SetPriority(Thread* thread, double rate);

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
    dword maxPriority;
    dword totalTick;
};

#endif
