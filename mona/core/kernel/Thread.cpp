/*
 * Thread.cpp - 
 *
 *   Copyright (c) 2009  Higepon(Taro Minowa)  <higepon@users.sourceforge.jp>
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 *   TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  $Id: Thread.cpp 183 2008-07-04 06:19:28Z higepon $
 */

#include "global.h"
#include "sys/HList.h"
#include "Thread.h"


Thread::Thread() : 
    priority(ThreadPriority::Normal),
    basePriority(ThreadPriority::Normal),
    index(0),
    tinfo(NULL),
    messageList(HList<MessageInfo*>()),
    timerMessage(NULL),
    id(0),
    lastCpuUsedTick(0),
    age(0),
    schedulerTotalTick(0),
    flags(0),
    wakeupSleep(0),
    stackSegment(NULL),
    kernelStackBottom(0),
    observers(2),
    waitingMutex_(NULL),
    waitingCondition_(NULL),
    isAlive_(true)
{
    clearEventWaiting();
    /* thread information */
    tinfo = new ThreadInfo;
    MONA_ASSERT(tinfo);
    tinfo->thread = this;

    /* thread information arch dependent */
    tinfo->archinfo = new ArchThreadInfo;
    MONA_ASSERT(tinfo->archinfo);
}

Thread::Thread(const Thread& src) :
    Node(src),
    priority(src.priority),
    basePriority(src.basePriority),
    index(src.index),
    tinfo(src.tinfo),
    messageList(src.messageList),
    id(src.id),
    lastCpuUsedTick(src.lastCpuUsedTick),
    age(src.age),
    schedulerTotalTick(src.schedulerTotalTick),
    flags(src.flags),
    wakeupSleep(src.wakeupSleep),
    stackSegment(src.stackSegment),
    kernelStackBottom(src.kernelStackBottom),
    observers(src.observers),
    waitingMutex_(src.waitingMutex_),
    waitingCondition_(src.waitingCondition_),
    isAlive_(true)
{
    MONA_ASSERT(false);
}

Thread& Thread::operator=(const Thread& src)
{
    MONA_ASSERT(false);
    basePriority = src.basePriority;
    return *this;
}

Thread::~Thread()
{
    isAlive_ = false;
    for (int i = 0; i < messageList.size(); i++) {
        delete messageList[i];
    }

    delete tinfo->archinfo;
    delete tinfo;
}

void Thread::setReturnValue(intptr_t value)
{
    tinfo->archinfo->eax = (uint32_t)value;
}

