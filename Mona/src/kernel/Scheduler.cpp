#include "Scheduler.h"
#include "Process.h"
#include "global.h"

/*----------------------------------------------------------------------
    Scheduler thanks Yaneurao.
----------------------------------------------------------------------*/
Scheduler::Scheduler() : runq(ThreadPriority::Min + 1), waitq(3), totalTick(0)
{
    for (int i = 0; i < runq.GetLength(); i++)
    {
        runq[i] = new Thread();

        ASSERT(runq[i]);

        runq[i]->Initialize();
    }

    for (int i = 0; i < waitq.GetLength(); i++)
    {
        waitq[i] = new Thread();

        ASSERT(waitq[i]);

        waitq[i]->Initialize();
    }

    maxPriority = runq.GetLength();
}

Scheduler::~Scheduler()
{
}

bool Scheduler::Schedule1()
{
    /* set priority and set to the queue */
    SetPriority(g_currentThread->thread);

    MoveToNewPosition(runq, g_currentThread->thread);

    WakeupTimer();

    return SetNextThread();
}

void Scheduler::SwitchToNext()
{
    bool isProcessChanged = SetNextThread();
    ThreadOperation::switchThread(isProcessChanged, 6532);

    /* not reached */
}

bool Scheduler::Schedule2()
{
    FOREACH(Thread*, queue, runq)
    {
        FOREACH_N(queue, Thread*, thread)
        {
            /* already scheduled ? */
            if (this->totalTick == thread->schedulerTotalTick)
            {
                continue;
            }

            SetPriority(thread, 2);

            /* insert into runq[priority] */
            Thread* prev = (Thread*)(thread->prev);

            MoveToNewPosition(runq, thread);
            thread = prev;
        }
    }
    END_FOREACH

    WakeupTimer();

    return SetNextThread();
}

void Scheduler::SetPriority(Thread* thread, unsigned char rate)
{
    thread->lastCpuUsedTick = (dword)(thread->lastCpuUsedTick / rate);

    dword priority = thread->basePriority + thread->lastCpuUsedTick;

    if (priority >= maxPriority)
    {
        priority = maxPriority - 1;
    }

    thread->priority = priority;
    thread->schedulerTotalTick = this->totalTick;
}

void Scheduler::SetPriority(Thread* thread)
{
    dword priority = thread->basePriority + thread->lastCpuUsedTick;

    if (priority >= maxPriority)
    {
        priority = maxPriority - 1;
    }

    thread->priority = priority;
    thread->schedulerTotalTick = this->totalTick;
}

void Scheduler::WakeupTimer()
{
    FOREACH(Thread*, queue, waitq)
    {
        FOREACH_N(queue, Thread*, thread)
        {
            Thread* prev = (Thread*)(thread->prev);
            if (!WakeupTimer(thread)) continue;

            thread = prev;
        }
    }
    END_FOREACH
}

bool Scheduler::WakeupTimer(Thread* thread)
{
    ASSERT(thread);

    if (thread->waitEvent != MEvent::TIMER) return false;

    if (thread->wakeupTimer > this->totalTick) return false;

    thread->waitEvent = MEvent::NONE;
    thread->priority = MEvent::TIMER; // umm
    thread->lastCpuUsedTick = 0;

    MoveToNewPosition(runq, thread);

    return true;
}

bool Scheduler::SetNextThread()
{
    Thread* root = NULL;

    FOREACH(Thread*, queue, runq)
    {
        if (queue->IsEmpty())
        {
            continue;
        }
        else
        {
            root = queue;
            break;
        }
    }
    END_FOREACH

    g_prevThread = g_currentThread;

    ASSERT(root);

//    g_currentThread = (root == NULL) ? g_idleThread->tinfo : ((Thread*)(root->Top()))->tinfo;
    g_currentThread = ((Thread*)(root->Top()))->tinfo;

    return !(IN_SAME_SPACE(g_prevThread,  g_currentThread));
}

void Scheduler::Dump()
{
    FOREACH(Thread*, queue, runq)
    {
        if (queue->IsEmpty()) continue;

        FOREACH_N(queue, Thread*, thread)
        {
            logprintf("runq:%s(%d)\n", thread->tinfo->process->getName(), thread->priority);
        }
    }
    END_FOREACH

    FOREACH(Thread*, queue, waitq)
    {
        if (queue->IsEmpty()) continue;

        FOREACH_N(queue, Thread*, thread)
        {
            logprintf("waitq:%s(%d)\n", thread->tinfo->process->getName(), thread->priority);
        }
    }
    END_FOREACH
}

void Scheduler::Sleep(Thread* thread, dword tick)
{
    ASSERT(thread);
    ASSERT(tick > 0);

    dword now = this->totalTick;
    thread->wakeupTimer = now + tick;
    WaitEvent(thread, MEvent::TIMER);
}

void Scheduler::WaitEvent(Thread* thread, int waitEvent)
{
    ASSERT(thread);

    thread->waitEvent = waitEvent;

    thread->Remove();
    waitq[GetTargetQeueue(waitEvent)]->AddToPrev(thread);
}

int Scheduler::EventComes(Thread* thread, int waitEvent)
{
    ASSERT(thread);

    if (thread->waitEvent != waitEvent) return 0;

    thread->waitEvent = MEvent::NONE;
    thread->priority = waitEvent; // umm
    thread->lastCpuUsedTick = 0;

    MoveToNewPosition(runq, thread);

    return 1;
}

Process* Scheduler::FindProcess(dword pid)
{
    FOREACH(Thread*, queue, runq)
    {
        FOREACH_N(queue, Thread*, thread)
        {
            Process* process = thread->tinfo->process;

            if (process->getPid() == pid)
            {
                return process;
            }
        }
    }
    END_FOREACH

    FOREACH(Thread*, queue, waitq)
    {
        FOREACH_N(queue, Thread*, thread)
        {
            Process* process = thread->tinfo->process;

            if (process->getPid() == pid)
            {
                return process;
            }
        }
    }
    END_FOREACH

    return (Process*)NULL;
}

dword Scheduler::LookupMainThread(const char* name)
{
    Process* process = FindProcess(name);

    if (process == NULL)
    {
        return 0xFFFFFFFF;
    }

    List<Thread*>* list = process->getThreadList();
    dword found = 0xFFFFFFFF;

    for (int i = 0; i < list->size(); i++)
    {
        dword id = list->get(i)->id;

        if (id < found)
        {
            found = id;
        }
    }
    return found;
}


Thread* Scheduler::Find(dword id)
{
    FOREACH(Thread*, queue, runq)
    {
        FOREACH_N(queue, Thread*, thread)
        {
            if (id == thread->id) return thread;
        }
    }
    END_FOREACH

    FOREACH(Thread*, queue, waitq)
    {
        FOREACH_N(queue, Thread*, thread)
        {
            if (id == thread->id) return thread;
        }
    }
    END_FOREACH

    return (Thread*)NULL;
}

dword Scheduler::Lookup(const char* name)
{
    Process* process = FindProcess(name);

    if (process == (Process*)NULL)
    {
        return 0xFFFFFFFF;
    }
    else
    {
        return process->getPid();
    }
}

int Scheduler::Kill(Thread* thread)
{
    List<Thread*>* list = thread->tinfo->process->getThreadList();
    list->remove(thread);
    thread->Remove();
    return NORMAL;
}

void Scheduler::SetDump()
{
    g_ps.next = NULL;

    PsInfo* current = &g_ps;

    FOREACH(Thread*, queue, runq)
    {
        FOREACH_N(queue, Thread*, thread)
        {
            ThreadInfo* i = thread->tinfo;

            current->next = new PsInfo;
            current  = current->next;

            strncpy(current->name, i->process->getName(), sizeof(current->name));
            current->cr3   = i->archinfo->cr3;
            current->eip   = i->archinfo->eip;
            current->esp   = i->archinfo->esp;
            current->tid   = thread->id;
            current->state = 1;
        }
    }
    END_FOREACH

    FOREACH(Thread*, queue, waitq)
    {
        FOREACH_N(queue, Thread*, thread)
        {
           ThreadInfo* i = thread->tinfo;

            current->next = new PsInfo;
            current  = current->next;

            strncpy(current->name, i->process->getName(), sizeof(current->name));
            current->cr3   = i->archinfo->cr3;
            current->eip   = i->archinfo->eip;
            current->esp   = i->archinfo->esp;
            current->tid   = thread->id;
            current->state = 0;
        }
    }
    END_FOREACH

    dumpCurrent = g_ps.next;
    current->next = NULL;
}

PsInfo* Scheduler::ReadDump()
{
    if (dumpCurrent == NULL)
    {
        return NULL;
    }

    PsInfo* info = dumpCurrent;
    dumpCurrent = dumpCurrent->next;
    return info;
}

dword* Scheduler::GetAllThreadID(dword* threadNum)
{
    dword* result;
    dword i     = 0;
    dword count = 0;

    FOREACH(Thread*, queue, runq)
    {
        FOREACH_N(queue, Thread*, thread)
        {
            count++;
        }
    }
    END_FOREACH

    FOREACH(Thread*, queue, waitq)
    {
        FOREACH_N(queue, Thread*, thread)
        {
            count++;
        }
    }
    END_FOREACH

    result = new dword[count];
    if (result == NULL) return NULL;

    FOREACH(Thread*, queue, runq)
    {
        FOREACH_N(queue, Thread*, thread)
        {
            result[i] = thread->id;
            i++;
        }
    }
    END_FOREACH

    FOREACH(Thread*, queue, waitq)
    {
        FOREACH_N(queue, Thread*, thread)
        {
            result[i] = thread->id;
            i++;
        }
    }
    END_FOREACH

    *threadNum = count;
    return result;
}

Process* Scheduler::FindProcess(const char* name)
{
    FOREACH(Thread*, queue, runq)
    {
        FOREACH_N(queue, Thread*, thread)
        {
            Process* process = thread->tinfo->process;

            if (!strcmp(name, process->getName()))
            {
                return process;
            }
        }
    }
    END_FOREACH

    FOREACH(Thread*, queue, waitq)
    {
        FOREACH_N(queue, Thread*, thread)
        {
            Process* process = thread->tinfo->process;

            if (!strcmp(name, process->getName()))
            {
                return process;
            }
        }
    }
    END_FOREACH

    return (Process*)NULL;
}
