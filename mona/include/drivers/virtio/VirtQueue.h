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

#ifndef _VIRTQUEUE_
#define _VIRTQUEUE_

#include <drivers/virtio/VirtBuffer.h>
#include <vector>

class VirtioDevice;
class VirtQueue
{
private:
    VirtQueue(uint16_t queueIndex, VirtioDevice& dev);

public:
    virtual ~VirtQueue()
    {
        deactivate();
        delete mem_;
        delete [] requestCookies_;
    }

    void* getBuf(int& size)
    {
        if (!isUsedBufExist()) {
            return NULL;
        }

        int headBufIndex = vring_.used->ring[lastUsedIndex_ % vring_.num].id;
        size = vring_.used->ring[lastUsedIndex_ % vring_.num].len;
        ASSERT(headBufIndex < (uintptr_t)vring_.num);
        lastUsedIndex_++;
        void* ret = (void*)requestCookies_[headBufIndex];
        requestCookies_[headBufIndex] = NULL;

        int i = headBufIndex;
        while (vring_.desc[i].flags & VRING_DESC_F_NEXT) {
            freeDescCount_++;
            i = vring_.desc[i].next;
        }

        vring_.desc[i].next = freeHeadIndex_;
        freeHeadIndex_ = headBufIndex;
        freeDescCount_++;
        return ret;
    }

    intptr_t addBuf(const std::vector<VirtBuffer>& out, const std::vector<VirtBuffer>& in, void* cookie)
    {
        uintptr_t bufCount = in.size() + out.size();
        if (bufCount == 0 || freeDescCount_ < bufCount) {
            return M_NO_SPACE;
        }
        freeDescCount_ -= bufCount;
        const int descToAddIndex = freeHeadIndex_;

        int i = freeHeadIndex_;
        for (std::vector<VirtBuffer>::const_iterator it = out.begin(); it != out.end(); ++it, ++i) {
            vring_.desc[i].addr = (*it).getPhysicalAddress();
            vring_.desc[i].len = (*it).length;
            vring_.desc[i].flags = VRING_DESC_F_NEXT;
        }
        for (std::vector<VirtBuffer>::const_iterator it = in.begin(); it != in.end(); ++it, ++i) {
            vring_.desc[i].addr = (*it).getPhysicalAddress();
            vring_.desc[i].len = (*it).length;
            vring_.desc[i].flags = VRING_DESC_F_NEXT | VRING_DESC_F_WRITE;
        }
        vring_.desc[i - 1].flags &= ~VRING_DESC_F_NEXT;
        freeHeadIndex_ = i;
        // Just set desc to avail ring, don't change vring_.avail->idx
        vring_.avail->ring[(vring_.avail->idx + addedBufCount_) % vring_.num] = descToAddIndex;
        requestCookies_[descToAddIndex] = cookie;
        addedBufCount_++;
        return M_OK;
    }

    void kick();
    static VirtQueue* findVirtQueue(int queueIndex, VirtioDevice& dev);

// public for testability
public:
    struct vring& getVring()
    {
        return vring_;
    }

    uintptr_t getFreeDescCount() const
    {
        return freeDescCount_;
    }

    uintptr_t getAddedBufCount() const
    {
        return addedBufCount_;
    }

    bool isUsedBufExist() const
    {
        writeMemoryBarrier();
        return vring_.used->idx != lastUsedIndex_;
    }

private:
    void writeMemoryBarrier() const
    {
        __asm__ __volatile__("" : : : "memory");
    }
    void deactivate();
    void activate(uintptr_t paddr);

    uint16_t queueIndex_;
    struct vring vring_;
    uintptr_t freeDescCount_;
    uintptr_t freeHeadIndex_;
    uintptr_t lastUsedIndex_;
    uintptr_t addedBufCount_;
    VirtioDevice& dev_;
    ContigousMemory* mem_;
    void** requestCookies_;
};

#endif // _VIRTQUEUE_
