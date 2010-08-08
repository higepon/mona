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

#include <drivers/virtio/VirtioDevice.h>
#include <drivers/virtio/VirtQueue.h>

VirtQueue::VirtQueue(uint16_t queueIndex, VirtioDevice& dev) :
    queueIndex_(queueIndex),
    freeDescCount_(dev.inp16(VIRTIO_PCI_QUEUE_NUM)),
    freeHeadIndex_(0),
    lastUsedIndex_(0),
    addedBufCount_(0),
    dev_(dev)
{
    ASSERT(freeDescCount_ > 0);
    mem_ = ContigousMemory::allocate(vring_size(freeDescCount_, MAP_PAGE_SIZE));
    ASSERT(mem_);
    _logprintf("mem=%x\n", mem_->getPhysicalAddress());

    vring_init(&vring_, freeDescCount_, mem_->get(), MAP_PAGE_SIZE);
    activate(mem_->getPhysicalAddress());

    requestCookies = new void*[freeDescCount_];

// for debug
#if 1
    for (uintptr_t i = 0; i < freeDescCount_ - 2; i++) {
        vring_.desc[i].next = i + 1;
        requestCookies[i] = NULL;
    }
    vring_.desc[vring_.num - 2].next = 0xff;

#else
    for (uintptr_t i = 0; i < freeDescCount_ - 1; i++) {
        vring_.desc[i].next = i + 1;
        requestCookies[i] = NULL;
    }
#endif
}

void VirtQueue::activate(uintptr_t paddr)
{
    dev_.outp32(VIRTIO_PCI_QUEUE_PFN, paddr >> VIRTIO_PCI_QUEUE_ADDR_SHIFT);
}

void VirtQueue::deactivate()
{
    dev_.outp32(VIRTIO_PCI_QUEUE_PFN, NULL);
}

VirtQueue* VirtQueue::findVirtQueue(int queueIndex, VirtioDevice& dev)
{
    // Select the queue to use.
    dev.outp16(VIRTIO_PCI_QUEUE_SEL, queueIndex);

    // How many descriptors do the queue have?
    const int numberOfDesc = dev.inp16(VIRTIO_PCI_QUEUE_NUM);
    if (numberOfDesc == 0) {
        return NULL;
    }

    // already activated?
    if (dev.inp32(VIRTIO_PCI_QUEUE_PFN)) {
        return NULL;
    }
    return new VirtQueue(queueIndex, dev);
}

void VirtQueue::kick()
{
//    _logprintf("vring_.avail->idx=%d %d:%d\n", vring_.avail->idx, addedBufCount_, __LINE__);
    vring_.avail->idx += addedBufCount_;
//    _logprintf("vring_.avail->idx=%d %d:%d\n", vring_.avail->idx, addedBufCount_, __LINE__);
    addedBufCount_ = 0;

    if (vring_.used->flags & VRING_USED_F_NO_NOTIFY) {
//        _logprintf("no notify %s %s:%d\n", __func__, __FILE__, __LINE__);
        return;
    } else {
//        _logprintf("notify %s %s:%d\n", __func__, __FILE__, __LINE__);
        dev_.outp16(VIRTIO_PCI_QUEUE_NOTIFY, queueIndex_);
    }
}
