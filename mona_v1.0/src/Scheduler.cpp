/*!
    \file  Scheduler.cpp
    \brief process scheduler

    Copyright (c) 2002,2003 Higepon
    All rights reserved.
    License=MIT/X Licnese

    \author  HigePon
    \version $Revision$
    \date   create:2003/06/28 update:$Date$
*/

#include <Scheduler.h>

ProcessInfo Scheduler::dispatchList_;
ProcessInfo Scheduler::sleepList_;

Scheduler::Scheduler(Process* idle) {

    dispatchList_.prev = &dispatchList_;
    dispatchList_.next = &dispatchList_;

    sleepList_.prev = &sleepList_;
    sleepList_.next = &sleepList_;

    idle_ = idle;

    tick_ = 0;
}

void Scheduler::addToPrev(ProcessInfo* list, ProcessInfo* process) {

    process->prev    = list->prev;
    process->next    = list;
    list->prev->next = process;
    list->prev       = process;
}

void Scheduler::removeFrom(ProcessInfo* process) {

    process->prev->next = process->next;
    process->next->prev = process->prev;
}

bool Scheduler::isEmpty(ProcessInfo* list) {

    if (list->next == list) return true;
    return false;
}

ProcessInfo* Scheduler::getNext(ProcessInfo* list) {

    ProcessInfo* result;

    result      = list->next;
    list->next  = result->next;
    result->next->prev = list;
    return result;
}

void Scheduler::schedule() {

    if (g_console) g_console->printf("schdule");

    this->wakeup();

    if (g_current_process->state != Process::SLEEPING) {
        g_current_process->state = Process::READY;
    }

    if (isEmpty(&dispatchList_)) {
        g_current_process = &(idle_->pinfo_);
        return;
    }

    ProcessInfo* next = getNext(&dispatchList_);

    toUserMode_ = next->dpl > 0;

    addToPrev(&dispatchList_, next);

    g_current_process = next;

    return;
}

bool Scheduler::toUserMode() const {

    return toUserMode_;
}

void Scheduler::addProcess(ProcessInfo* process) {

    addToPrev(&dispatchList_, process);
}

void Scheduler::wakeup() {

    ProcessInfo* start;
    dword tick = g_process_manager->getTick();

    for (start = sleepList_.next; start != &sleepList_; start = start->next) {

        if (start->wakeupTimer <= tick) {

            start->state = Process::READY;

            ProcessInfo* prev = start->prev;

            removeFrom(start);
            addToPrev(&dispatchList_, start);
            start = prev;
        }
    }
}

void Scheduler::sleep(ProcessInfo* process, dword tick) {

    process->wakeupTimer = getTick() + tick;
    removeFrom(process);
    addToPrev(&sleepList_, process);
}

void Scheduler::tick() {

    tick_ += 10;
}

dword Scheduler::getTick() {

    return tick_;
}

bool Scheduler::kill(ProcessInfo* process) {

    /* check lock and existence */
    /* not implemented          */

    removeFrom(process);
    delete(process->process);

    return true;
}
