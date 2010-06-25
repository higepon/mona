/*
 * Buffer.h - A class represents a contiguous physical memory.
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
 *  $Id: Buffer.h 261 2008-07-25 06:16:44Z higepon $
 */

#ifndef MONA_CONTIGOUS_PHYSICAL_MEMORY_
#define MONA_CONTIGOUS_PHYSICAL_MEMORY_

#include "virtio.h"

#define CHECK_BUFFER(buf) buf.check(__FILE__, __LINE__)

class ContigousPhysicalMemory {
public:
    ContigousPhysicalMemory(uintptr_t size) :
        size_(size + 1),
        pageNum_((size_ + PAGE_SIZE - 1) / PAGE_SIZE),
        lastError_(M_OK)
    {
        uintptr_t address = startAddress;
        lastError_ = syscall_allocate_contiguous(address, pageNum_);
        if (lastError_ != M_OK) {
            return;
        }
        startAddress += pageNum_ * PAGE_SIZE;
        data_ = (uint8_t*)address;
        memset(data_, 0, size_);
        data_[size_ - 1] = 0xcc;
    }

    virtual ~ContigousPhysicalMemory() {}

    void check(const char* file, int line) const
    {
        if (data_[size_ - 1] != 0xcc) {
            _printf("ASSERT %s:%d\n", file, line);
            exit(-1);
        }
    }

    uint8_t* data() const { return data_; }
    uintptr_t size() const { return size_; }
    intptr_t getLastError() const { return lastError_; }

private:
    uintptr_t size_;
    int pageNum_;
    uint8_t* data_;
    intptr_t lastError_;
    static uintptr_t startAddress;
};

#endif // MONA_CONTIGOUS_PHYSICAL_MEMORY_
