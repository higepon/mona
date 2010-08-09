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

#ifndef _VIRTIOBLOCK_
#define _VIRTIOBLOCK_

#include <monapi/ContigousMemory.h>
#include <drivers/virtio/VirtQueue.h>
#include <vector>

class VirtioBlock
{

// Posible enhancement
//   We can issue multiple requests, and wait first response to come.
//   But for now, since file_server requests are serialized, device requsts are also serialized.
private:
    VirtioBlock(VirtioDevice* vdev) : vdev_(vdev)
    {
        ASSERT(vdev_ != NULL);
        vq_ = vdev_->findVirtQueue(0);
        ASSERT(vq_ != NULL);
    }

public:
    virtual ~VirtioBlock()
    {
        ASSERT(vq_);
        delete vq_;
        ASSERT(vdev_ != NULL);
        delete vdev_;
    }

    int64_t read(uint8_t* readBuf, int64_t sector, int64_t sizeToRead)
    {
        ContigousMemory* mem = ContigousMemory::allocate(4096);
        if (mem == NULL) {
            return M_NO_MEMORY;
        }
        struct virtio_blk_outhdr* hdr = (struct virtio_blk_outhdr*)mem->get();

        std::vector<VirtBuffer> out;
        hdr->type = VIRTIO_BLK_T_IN ;
        hdr->ioprio = 0;
        hdr->sector = sector;
        out.push_back(VirtBuffer(hdr, sizeof(struct virtio_blk_outhdr)));

        std::vector<VirtBuffer> in;
        uint8_t* status = (uint8_t*)((uintptr_t)mem->get() + sizeof(struct virtio_blk_outhdr));
        *status = 0xff;

        uint8_t* buf = (uint8_t*)((uintptr_t)mem->get() + sizeof(struct virtio_blk_outhdr) + 1);
        memset(buf, 0, sizeToRead);
        in.push_back(VirtBuffer(buf, sizeToRead));

        in.push_back(VirtBuffer(status, 1));

        intptr_t addBufRet = vq_->addBuf(out, in, (void*)0xdeadbeaf);
        if (addBufRet != M_OK) {
            return addBufRet;
        }

        vq_->kick();
        while (!vq_->isUsedBufExist()) {
        }

        int sizeRead = 0;
        void* cookie = vq_->getBuf(sizeRead);
        if (*status != VIRTIO_BLK_S_OK) {
            return M_READ_ERROR;
        }
        ASSERT(0xdeadbeaf == (uintptr_t)cookie);
        ASSERT(sizeRead <= sizeToRead);
        memcpy(readBuf, buf, sizeRead);
        delete mem;
        return sizeRead;
    }

    uint64_t getSectorSize() const
    {
        return 512;
    }

    uint64_t getCapacity() const
    {
        struct virtio_blk_config config;
        vdev_->getConfig(&config, 0, sizeof(struct virtio_blk_config));
        return config.capacity;
    }

    static VirtioBlock* probe(int deviceIndex)
    {
        VirtioDevice* vdev = VirtioDevice::probe(PCI_DEVICE_ID_VIRTIO_BLOCK, deviceIndex);
        if (vdev == NULL) {
            return NULL;
        } else {
            return new VirtioBlock(vdev);
        }
    }

private:
    VirtioDevice* vdev_;
    VirtQueue* vq_;
};

#endif // _VIRTIOBLOCK_
