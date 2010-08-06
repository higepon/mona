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
 */

#ifndef _CONTIGOUSMEMORY_
#define _CONTIGOUSMEMORY_

#include <sys/types.h>
#include <monapi.h>
#include <monapi/Assert.h>
#include <sys/BitMap.h>

namespace MonAPI {

class ContigousMemory
{
private:
    ContigousMemory(uintptr_t laddress, int pageStart, int pageNum) :
        laddress_(laddress),
        pageStart_(pageStart),
        pageNum_(pageNum)
    {
    }

    uintptr_t laddress_;
    int pageStart_;
    int pageNum_;
    static const uintptr_t START_ADDRESS = 0x9fc00000;
    static const uintptr_t MAX_PAGES = (4 * 1024 * 1024) / MAP_PAGE_SIZE; // 4MB
    static BitMap* pagesMap;

public:

    static BitMap* getPagesMap()
    {
        if (NULL == pagesMap) {
            pagesMap = new BitMap(MAX_PAGES);
        }
        return pagesMap;
    }

    virtual ~ContigousMemory()
    {
        syscall_deallocate_contiguous(laddress_, pageNum_);
        for (int i = 0; i < pageNum_; i++) {
            getPagesMap()->clear(pageStart_ + i);
        }
    }

    static ContigousMemory* allocate(int size)
    {
        int pageNum = (size + MAP_PAGE_SIZE - 1) / MAP_PAGE_SIZE;
        int found = getPagesMap()->find(pageNum);
        if (found == -1) {
            _logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
            return NULL;
        }
        uintptr_t laddress = START_ADDRESS + (found * MAP_PAGE_SIZE);
        if (M_OK != syscall_allocate_contiguous(laddress, pageNum)) {
            for (int i = 0; i < pageNum; i++) {
                getPagesMap()->clear(i + found);
            }
            _logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
            return NULL;
        }
        memset((uint8_t*)laddress, 0, pageNum * MAP_PAGE_SIZE);
        return new ContigousMemory(laddress, found, pageNum);
    }

    void* get() const
    {
        return (void*)laddress_;
    }

    uintptr_t getPhysicalAddress() const
    {
        return syscall_get_physical_address(laddress_);
    }
};

} // namespace MonaAPI

#endif // _CONTIGOUSMEMORY_
