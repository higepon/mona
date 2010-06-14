/*
 *   Copyright (c) 2010  Higepon(Taro Minowa)  <higepon@users.sourceforge.jp>
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
 */

#ifndef _KOBJECT_SERVICE_
#define _KOBJECT_SERVICE_

#include "sys/types.h"

class KMutex;
class Process;

class KObjectService
{
private:
    KObjectService() {}
    ~KObjectService() {}
public:
    static void cleanupKObjects(Process* owner);

    static intptr_t createUserSemaphore(Process* owner, int num)
    {
        UserSemaphore* obj = new UserSemaphore(num);
        return g_id->allocateID(owner, obj);
    }

    static intptr_t markAsShared(Process* owner, KObject* obj)
    {
       return g_id->allocateID(owner, obj);
    }

    static intptr_t createTimer(KTimer** timer, Process* owner, Thread* thread, int tick)
    {
        KTimer* obj = new KTimer(thread, tick);
        *timer = obj;
        return g_id->allocateID(owner, obj);;
    }

    template <typename T> static intptr_t create(Process* owner)
    {
        T* obj = new T();
        return g_id->allocateID(owner, obj);
    }

    // A mutex which has null owner will never deleted.
    template <typename T> static intptr_t createNullOwner()
    {
        return create<T>(NULL);
    }

    static bool destroy(intptr_t id, KObject* obj);
};

#endif // _KOBJECT_SERVICE_
