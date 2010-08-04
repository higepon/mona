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
public:
    VirtQueue(ContigousMemory* mem, int numberOfDesc, VirtioDevice& dev) : freeNum_(numberOfDesc), dev_(dev)
    {
        vring_init(&vring_, numberOfDesc, mem->get(), MAP_PAGE_SIZE);
    }

    virtual ~VirtQueue()
    {
        deactivate();
    }

    int getFreeNum() const
    {
        return freeNum_;
    }

    intptr_t addBuf(const std::vector<VirtBuffer>& in, const std::vector<VirtBuffer>& out, void* cookie)
    {
        freeNum_ -= in.size() + out.size();
        return M_OK;
    }

private:
    void deactivate();

    struct vring vring_;
    int freeNum_;
    VirtioDevice& dev_;
};

#endif // _VIRTQUEUE_
