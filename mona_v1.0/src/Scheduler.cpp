/*!
    \file  Scheduler.cpp
    \brief process scheduler

    Copyright (c) 2003 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2003/06/28 update:$Date$
*/

#include <Scheduler.h>
#include <global.h>

ProcessInfo Scheduler::list_;

void Scheduler::setup() {

    list_.prev = &list_;
    list_.next = &list_;
}

void Scheduler::addToPrev(ProcessInfo* process) {

    process->prev    = list_.prev;
    process->next    = &list_;
    list_.prev->next = process;
    list_.prev       = process;
}

void Scheduler::removeFrom(ProcessInfo* process) {

    process->prev->next = process->next;
    process->next->prev = process->prev;
}

bool Scheduler::isEmpty() {

    if (list_.next == &list_) return true;
    return false;
}

ProcessInfo* Scheduler::getNext() {

    ProcessInfo* result;

    result     = list_.next;
    list_.next = result->next;
    result->next->prev = &list_;
    return result;
}

void Scheduler::schedule() {

    ProcessInfo* temp = getNext();

    addToPrev(temp);

    g_current_process = temp;

    /* switch */

    return;
}
