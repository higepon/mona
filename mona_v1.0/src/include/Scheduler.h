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

class Scheduler {

  public:
    void setup();
    void addToPrev(ProcessInfo* process);
    void removeFrom(ProcessInfo* process);
    bool isEmpty();
    ProcessInfo* getNext();
    void schedule();

  public:
    static ProcessInfo list_;
};

#endif
