#ifndef _MONA_SCHEDULER_
#define _MONA_SCHEDULER_

#include "Thread.h"
#include "Array.h"
#include "Process.h"
#include "sys/ThreadPriority.h"

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
    void SwitchToNext();

    void Join(Thread* thread)
    {
        Join(thread, ThreadPriority::Normal);
    }

    void Join(Thread* thread, dword basePriority)
    {
        thread->basePriority = basePriority >= maxPriority ? maxPriority - 1 : basePriority;
        thread->priority     = thread->basePriority;


        ASSERT(thread->priority < maxPriority);

        runq[thread->priority]->AddToPrev(thread);
    }

    void ChangeBasePriority(Thread* thread, dword basePriority)
    {
        ASSERT(thread);

        if(thread->basePriority >= maxPriority)
        {
            thread->basePriority = maxPriority - 1;
        }

        thread->priority = basePriority;
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
    dword maxPriority;
    dword totalTick;
};

#endif
