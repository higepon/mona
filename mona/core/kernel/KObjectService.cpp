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

#include "global.h"
#include "KObjectService.h"
#include "KObject.h"
#include "Mutex.h"
#include "syscalls.h"
#include "io.h"
#include "Condition.h"

bool KObjectService::destroy(intptr_t id, KObject* obj)
{
    if (obj->getOwner() != NULL) {
        obj->getOwner()->removeKObject(id, obj);
    }
    // try delete by reference counting.
    return tryDelete(id, obj);
}

bool KObjectService::tryDelete(intptr_t id, KObject* obj)
{
    if (g_id->returnID(id)) {
        delete obj;
        return true;
    } else {
        return false;
    }
}

void KObjectService::cleanupKObjects(Process* owner)
{
    HList< Pair<intptr_t, KObject*> >* kobjects = owner->getKObjects();
    int size = kobjects->size();
    for (int i = 0; i < size; i++) {
        Pair<intptr_t, KObject*> p;
        bool isDeleted = kobjects->removeAt(0, &p);
        ASSERT(isDeleted);
        KObjectService::tryDelete(p.car, p.cdr);
    }
}
