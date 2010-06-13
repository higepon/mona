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
 *
 *  $Id: KObjectService.cpp 183 2008-07-04 06:19:28Z higepon $
 */

#include "KObjectService.h"
#include "global.h"
#include "KObject.h"
#include "Mutex.h"

static Thread* targetThread = NULL;
static Process* targetProcess = NULL;


static void cleanupKObject2(int id, KObject* obj)
{
    if (obj->getOwner() != targetThread) {
        return;
    }

    if (obj->getType() == KObject::KMUTEX) {
        KObjectService::destroyMutex(id, (KMutex*)obj);
    }
}

static void cleanupKObject(int id, KObject* obj)
{
    if (obj->getOwnerProcess() != targetProcess) {
        return;
    }

    if (obj->getType() == KObject::KMUTEX) {
        KObjectService::destroyMutex(id, (KMutex*)obj);
    }
}


bool KObjectService::destroyMutex(intptr_t id, KMutex* mutex)
{
    // Use reference counting.
    if (g_id->returnID(id)) {
        delete mutex;
        return true;
    } else {
        return false;
    }
}

void KObjectService::cleanupKObjects(Thread* owner)
{
    targetThread = owner;
    g_id->foreachKObject(&cleanupKObject2);
}

void KObjectService::cleanupKObjects(Process* owner)
{
    logprintf("higehige");
    targetProcess = owner;
    g_id->foreachKObject(&cleanupKObject);
}


intptr_t KObjectService::createMutex(Thread* owner)
{
    KMutex* mutex = new KMutex();
    logprintf("allocate mutex=%x %x\n",mutex, owner);
    return g_id->allocateID(owner, mutex);
}

// A mutex which has null owner will never deleted.
intptr_t KObjectService::createMutexNullOwner()
{
    return createMutex(NULL);
}
