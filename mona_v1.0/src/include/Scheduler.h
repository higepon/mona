/*!
    \file  Scheduler.h
    \brief scheduler

    Copyright (c) 2003 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2003/06/29 update:$Date$
*/
#ifndef _MONA_SCHEDULER_
#define _MONA_SCHEDULER_

#include <Process.h>
#include <global.h>

class Scheduler {

  public:
    Scheduler();
    void schedule();
    void sleep(ProcessInfo* process, dword tick);
    bool toUserMode() const;
    void addProcess(ProcessInfo* process);
    void tick();
    dword getTick();

  private:
    void wakeup();
    void removeFrom(ProcessInfo* process);
    bool isEmpty(ProcessInfo* list);
    ProcessInfo* getNext(ProcessInfo* list);
    void addToPrev(ProcessInfo* list, ProcessInfo* process);

  public:
    static ProcessInfo dispatchList_;
    static ProcessInfo sleepList_;

  private:
    bool toUserMode_;
    dword tick_;
};

#endif
