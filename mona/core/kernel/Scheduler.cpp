#include "io.h"
#include "Scheduler.h"
#include "Process.h"
#include "sys/error.h"
#include "syscalls.h"
#include "Condition.h"
#include "KObjectService.h"
#include "global.h"
#include "Thread.h"
/*----------------------------------------------------------------------
    Scheduler thanks Yaneurao.
----------------------------------------------------------------------*/
Scheduler::Scheduler() : runq(ThreadPriority::Min + 1), waitq(3), totalTick(0), reservedTid_(0)
{
    for (int i = 0; i < runq.GetLength(); i++)
    {
        runq[i] = new Thread();

        MONA_ASSERT(runq[i]);

        runq[i]->Initialize();
    }

    for (int i = 0; i < waitq.GetLength(); i++)
    {
        waitq[i] = new Thread();

        MONA_ASSERT(waitq[i]);

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
    WakeupSleep();

    return SetNextThread();
}

void Scheduler::SwitchToNext()
{
    bool isProcessChanged = SetNextThread();
    ThreadOperation::switchThread(isProcessChanged);

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
    WakeupSleep();

    return SetNextThread();
}

void Scheduler::SetPriority(Thread* thread, unsigned char rate)
{
    thread->lastCpuUsedTick = (uint32_t)(thread->lastCpuUsedTick / rate);

    uint32_t priority = thread->basePriority + thread->lastCpuUsedTick;

    if (priority >= maxPriority)
    {
        priority = maxPriority - 1;
    }

    thread->priority = priority;
    thread->schedulerTotalTick = this->totalTick;
}

void Scheduler::SetPriority(Thread* thread)
{
    uint32_t priority = thread->basePriority + thread->lastCpuUsedTick;

    if (priority >= maxPriority)
    {
        priority = maxPriority - 1;
    }

    thread->priority = priority;
    thread->schedulerTotalTick = this->totalTick;
}

void Scheduler::WakeupTimer()
{
    for (int i = 0; i < timers.size(); i++)
    {
        KTimer* timer = timers[i];

        if (!timer->timer(this->totalTick)) continue;

        Thread* thread = timer->getOwnerThread();
        ASSERT_THREAD_IS_ALIVE(thread);

        MessageInfo msg;
        memset(&msg, 0, sizeof(MessageInfo));
        msg.header = MSG_TIMER;
        msg.arg1   = timer->getId();

        timer->setNextTimer(this->totalTick);

        if (g_messenger->send(thread, &msg)) {
            KTimer* timer;
            timers.removeAt(i, &timer);
            KObjectService::destroy(thread->tinfo->process, timer->getId(), timer);
            g_console->printf("Send failed %s:%d\n", __FILE__, __LINE__);
            //            continue;
        }
        if (timer->isOneShot()) {
          timers.remove(timer);
          KObjectService::destroy(thread->tinfo->process, timer->getId(), timer);
        }
    }
}

void Scheduler::WakeupSleep()
{
    FOREACH(Thread*, queue, waitq)
    {
        FOREACH_N(queue, Thread*, thread)
        {
            Thread* prev = (Thread*)(thread->prev);
            if (!WakeupSleep(thread)) continue;

            thread = prev;
            ASSERT_THREAD_IS_ALIVE(thread);
        }
    }
    END_FOREACH
}

bool Scheduler::WakeupSleep(Thread* thread)
{
    MONA_ASSERT(thread);
    int eventIndex = thread->isWaiting(MEvent::SLEEP);
    if (-1 == eventIndex) {
        return false;
    }

    if (thread->wakeupSleep > this->totalTick) {
        return false;
    }

    // When the thread is waiting both MEvent::MUTEX_UNLOCKED and MEvent::SLEEP,
    // we have to remove the thread from Mutex waitList.
    int mutexIndex = thread->isWaiting(MEvent::MUTEX_UNLOCKED);
    if (-1 != mutexIndex) {
        KMutex* waitingMutex = thread->getWaitingMutex();
        MONA_ASSERT(waitingMutex != NULL);
        thread->setWaitingMutex(NULL);
        bool removed = waitingMutex->removeFromWaitList(thread);
        MONA_ASSERT(removed);
    }

    int conditionIndex = thread->isWaiting(MEvent::CONDITION_NOTIFY);
    if (-1 != conditionIndex) {
        Condition* waitingCondition = thread->getWaitingCondition();
        MONA_ASSERT(waitingCondition != NULL);
        thread->setWaitingCondition(NULL);
        bool removed = waitingCondition->removeFromWaitList(thread);
        MONA_ASSERT(removed);
    }


    thread->priority = MEvent::SLEEP; // umm
    thread->lastCpuUsedTick = 0;

    thread->setReturnValue(M_TIMED_OUT);
    thread->eventsWaiting[eventIndex] = MEvent::NONE;
    MoveToNewPosition(runq, thread);
    return true;
}


bool Scheduler::SetNextThread()
{
    if(reservedTid_ != 0 && reservedTid_ == g_currentThread->thread->id) {
        static intptr_t mutex = KObjectService::create<KMutex>(g_currentThread->thread->tinfo->process);
        systemcall_mutex_lock(mutex);
        if(reservedTid_ != 0 && reservedTid_ == g_currentThread->thread->id) {
            reservedTid_ = 0;
            g_page_manager->showCurrentStackTrace();
        }
        systemcall_mutex_unlock(mutex);
    }


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

    MONA_ASSERT(root);

//    g_currentThread = (root == NULL) ? g_idleThread->tinfo : ((Thread*)(root->Top()))->tinfo;
    g_currentThread = ((Thread*)(root->Top()))->tinfo;

// check cpu usage
#if 0
    static int count = 0;
    count++;
    if (count % 30 == 0) {
        logprintf("tid=%x eip=%x\n", g_currentThread->thread->id, g_currentThread->thread->tinfo->archinfo->eip);
    }
#endif


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

intptr_t Scheduler::SetTimer(Thread* thread, uint32_t tick, bool isOneShot)
{
    Process* owner = thread->tinfo->process;
    KTimer* timer = NULL;
    intptr_t id = KObjectService::createTimer(&timer, owner, thread, tick, isOneShot);
    timers.add(timer);
    timer->setNextTimer(this->totalTick);
    return id;
}

intptr_t Scheduler::KillTimer(uint32_t id, Thread* thread)
{
    KObject* object = g_id->get(id, KObject::KTIMER);

    if (object == NULL) {
        return M_BAD_TIMER_ID;
    }

    KTimer* timer = (KTimer*)object;
    timers.remove(timer);
    KObjectService::destroy(thread->tinfo->process, id, object);
    return M_OK;
}

void Scheduler::Sleep(Thread* thread, uint32_t tick)
{
    MONA_ASSERT(thread);
    MONA_ASSERT(tick > 0);

    uint32_t now = this->totalTick;
    thread->wakeupSleep = now + tick;

    WaitEvent(thread, MEvent::SLEEP);
}

void Scheduler::WaitEvent(Thread* thread, int eventForWait)
{
    WaitEvent2(thread, eventForWait, MEvent::NONE);
}

void Scheduler::WaitEvent2(Thread* thread, int eventForWait1, int eventForWait2)
{
    MONA_ASSERT(thread);

    thread->eventsWaiting[0] = eventForWait1;
    thread->eventsWaiting[1] = eventForWait2;

    thread->Remove();
    waitq[GetTargetQeueue(eventForWait1)]->AddToPrev(thread);
}

int Scheduler::EventComes(Thread* thread, int eventForWait)
{
    MONA_ASSERT(thread);

    int eventIndex = thread->isWaiting(eventForWait);
    if (eventIndex == -1) {
        return 0;
    }

    // Tells the client what event comes.
    thread->setReturnValue(thread->eventsWaiting[eventIndex]);
    thread->clearEventWaiting();

    thread->priority = eventForWait; // umm
    thread->lastCpuUsedTick = 0;

    MoveToNewPosition(runq, thread);

    return 1;
}

Process* Scheduler::FindProcess(uint32_t pid)
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


uint32_t Scheduler::LookupMainThread(const char* name)
{
    Process* process = FindProcess(name);
    if (process == NULL) {
        return THREAD_UNKNOWN;
    }

    return process->findMainThreadId();
}

Thread* Scheduler::Find(uint32_t id)
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

uint32_t Scheduler::Lookup(const char* name)
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
    thread->tinfo->process->removeThread(thread);
    thread->Remove();
    for (int i = timers.size() - 1; i >= 0; i--)
    {
        KTimer* timer = timers[i];
        if (timer->getOwnerThread() == thread) {
          timers.removeAt(i);
        }
    }

    return M_OK;
}

PsInfo* Scheduler::GetAllDump()
{
    PsInfo* start = new PsInfo;
    PsInfo* current = start;
    PsInfo* prev = NULL;
    current->next = NULL;
    FOREACH(Thread*, queue, runq)
    {
        FOREACH_N(queue, Thread*, thread)
        {
            ThreadInfo* i = thread->tinfo;
            strncpy(current->name, i->process->getName(), sizeof(current->name));
            current->name[sizeof(current->name) - 1] = '\0';
            current->cr3   = i->archinfo->cr3;
            current->eip   = i->archinfo->eip;
            current->esp   = i->archinfo->esp;
            current->tid   = thread->id;
            current->state = 1;
            current->next = new PsInfo;
            prev = current;
            current  = current->next;
        }
    }
    END_FOREACH

    FOREACH(Thread*, queue, waitq)
    {
        FOREACH_N(queue, Thread*, thread)
        {
           ThreadInfo* i = thread->tinfo;

            strncpy(current->name, i->process->getName(), sizeof(current->name));
            current->cr3   = i->archinfo->cr3;
            current->eip   = i->archinfo->eip;
            current->esp   = i->archinfo->esp;
            current->tid   = thread->id;
            current->state = 0;
            current->next = new PsInfo;
            prev = current;
            current  = current->next;
        }
    }
    END_FOREACH
    if (prev != NULL) {
        delete prev->next;
        prev->next = NULL;
    }
    return start;
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

Thread** Scheduler::GetAllThread(uint32_t* threadNum)
{
    Thread** result;
    uint32_t i     = 0;
    uint32_t count = 0;

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

    result = new Thread*[count];
    if (result == NULL) return NULL;

    FOREACH(Thread*, queue, runq)
    {
        FOREACH_N(queue, Thread*, thread)
        {
            result[i] = thread;
            i++;
        }
    }
    END_FOREACH

    FOREACH(Thread*, queue, waitq)
    {
        FOREACH_N(queue, Thread*, thread)
        {
            result[i] = thread;
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
