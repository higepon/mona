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

#ifndef _VIRTIO_BLOCK_
#define _VIRTIO_BLOCK_

#include <drivers/virtio/virtio_blk.h>
#include <drivers/virtio/VirtQueue.h>
#include <drivers/virtio/VirtioDevice.h>
#include <vector>
#include <drivers/BlockCache.h>

class VirtioBlock
{

// Possible enhancement
//   We can issue multiple requests, and wait first response to come.
//   But for now, since file_server requests are serialized, device requsts are also serialized.
private:
    VirtioBlock(VirtioDevice* vdev) : vdev_(vdev), bc_(600000)
    {
        ASSERT(vdev_.get() != NULL);
        vq_.reset(vdev_->findVirtQueue(0));
        ASSERT(vq_.get() != NULL);
        vdev_->enableInterrupt();
    }

public:
    virtual ~VirtioBlock()
    {
    }

    int64_t write(const void* writeBuf, int64_t sector, int64_t sizeToWrite)
    {
//        logprintf("[write]%s %s:%d\n", __func__, __FILE__, __LINE__);
        // Possible enhancement
        //   For now, we allocate ContigousMemory for each time, we can elminate allocating buffer.
        //   writeBuf can be used directory using scatter gather system.

        scoped_ptr<ContigousMemory> mem(ContigousMemory::allocate(sizeToWrite + sizeof(virtio_blk_outhdr) + 1));
        if (mem.get() == NULL) {
            monapi_warn("ContigousMemory no space!");
            return M_NO_MEMORY;
        }
        struct virtio_blk_outhdr* hdr = (struct virtio_blk_outhdr*)mem->get();

        std::vector<VirtBuffer> out;
        hdr->type = VIRTIO_BLK_T_OUT;
        hdr->type |= VIRTIO_BLK_T_BARRIER;
        hdr->ioprio = 0;
        hdr->sector = sector;
        out.push_back(VirtBuffer(hdr, sizeof(struct virtio_blk_outhdr)));

        std::vector<VirtBuffer> in;
        volatile uint8_t* status = (uint8_t*)((uintptr_t)mem->get() + sizeof(struct virtio_blk_outhdr));
        *status = 0xff;
        uint8_t* buf = (uint8_t*)((uintptr_t)mem->get() + sizeof(struct virtio_blk_outhdr) + 1);
//        logprintf("dest=%x %s %s:%d\n", buf, __func__, __FILE__, __LINE__);
        memcpy(buf, writeBuf, sizeToWrite);
        out.push_back(VirtBuffer(buf, sizeToWrite));

        in.push_back(VirtBuffer(status, 1));

        static uintptr_t cookie = 1;
        cookie++;
        intptr_t addBufRet = vq_->addBuf(out, in, (void*)cookie);
        if (addBufRet != M_OK) {
            monapi_warn("addBuf failed");
            return addBufRet;
        }
        vq_->kick();

        waitWithBusyLoop(2000);

        // gave up busy loop, use expensive waitInterrupt.
        while (!vq_->isUsedBufExist()) {
            vdev_->waitInterrupt();
        }

        memoryBarrier();

        int sizeWritten = 0;
        void* afterCookie = vq_->getBuf(sizeWritten);

        // In theory, this busy loop is not necessary.
        // But on this driver, sometime after getBut is called, the status is not updated.
        // This may be a bug of the driver.
        while (*status == 0xff) {
//            _logprintf("waiting");
        }
        sizeWritten -= sizeof(*status);
        if (*status != VIRTIO_BLK_S_OK) {
            monapi_warn("write error=%d", *status);
            return M_WRITE_ERROR;
        }
        ASSERT((sizeToWrite % 512) == 0);
        ASSERT((sizeWritten % 512) == 0);
//        logprintf("cached by write sector=%d - %d\n", (int)sector, (int)sector + sizeWritten / 512);
        bc_.addRange(sector, sizeWritten / 512, writeBuf);
        ASSERT((uintptr_t)afterCookie == cookie);
        ASSERT(sizeWritten <= sizeToWrite);
        return sizeWritten;
    }

    int64_t read(void* readBuf, int64_t sector, int64_t sizeToRead)
    {
        // imakoko 正しい場所を cache して読んでいる気がする。読み込むサイズの問題？
        // todo remove
//        logprintf("dest=%x %s %s:%d\n", readBuf, __func__, __FILE__, __LINE__);
        memset(readBuf, '0', (int)sizeToRead);
//        logprintf("[read] sector=%d sizeToRead=%d %s %s:%d\n", (int)sector, (int)sizeToRead, __func__, __FILE__, __LINE__);
        const int MAX_CONTIGOUS_SIZE = 3 * 1024 * 1024;
        ASSERT(MAX_CONTIGOUS_SIZE % getSectorSize() == 0);

        uintptr_t numSectors = (sizeToRead - 1 + bc_.sectorSize()) / bc_.sectorSize();
        Caches caches;
        IORequests rest;
        bc_.getCacheAndRest(sector, numSectors, caches, rest);
//        logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
        for (Caches::const_iterator it = caches.begin(); it != caches.end(); ++it) {
//            ASSERT(false);
            if ((*it).sector() == (sector + numSectors - 1)) {
                // here is a bug on last sector 512
                if (((int)sizeToRead % 512)== 0) {
                //     logprintf("[1]Cache Hit sector=%d address=%x\n", (*it).sector(), (*it).get());
                // for (int i = 0; i < bc_.sectorSize(); i++) {
                //     logprintf("%c", ((uint8_t*)(*it).get())[i]);
                // }
                // logprintf("\n\n");

                memcpy((uint8_t*)readBuf + bc_.sectorSize() * ((*it).sector() - (int)sector), (*it).get(), bc_.sectorSize());
//                logprintf("dest=%x %s %s:%d\n", (uint8_t*)readBuf + bc_.sectorSize() * ((*it).sector() - (int)sector), __func__, __FILE__, __LINE__);
                } else {
//                logprintf("[2]Cache Hit sector=%d\n", (*it).sector());
                    memcpy((uint8_t*)readBuf + bc_.sectorSize() * ((*it).sector() - sector), (*it).get(), (int)sizeToRead % bc_.sectorSize());
                }
            } else {
                // logprintf("[3]Cache Hit sector=%d address=%x\n", (*it).sector(), (*it).get());
                // for (int i = 0; i < bc_.sectorSize(); i++) {
                //     logprintf("%c", ((uint8_t*)(*it).get())[i]);
                // }
                // logprintf("\n\n");
                memcpy((uint8_t*)readBuf + bc_.sectorSize() * ((*it).sector() - sector), (*it).get(), bc_.sectorSize());
//                logprintf("dest=%x %s %s:%d\n", (uint8_t*)readBuf + bc_.sectorSize() * ((*it).sector() - sector), __func__, __FILE__, __LINE__);
            }
        }
        // todo : cache here.
//        logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
        for (IORequests::iterator it = rest.begin(); it != rest.end(); ++it) {
//            logprintf("rest startSector=%d rest.numSectors=%d arg.numSectors=%d\n", (*it).startSector(),(*it).numSectors(), numSectors);
            int sizeToRead2 = 0;
            bool isLastSector = ((*it).startSector() + (*it).numSectors() == sector + numSectors);
            if (isLastSector) {
                //              logprintf("(int)sizeToRead % bc_.sectorSize() == 0 = %d\n", (int)sizeToRead % bc_.sectorSize() == 0);
                sizeToRead2 = ((*it).numSectors() - 1) * bc_.sectorSize() + ((int)sizeToRead % bc_.sectorSize() == 0 ? bc_.sectorSize() : (int)sizeToRead % bc_.sectorSize());
            } else {
                sizeToRead2 = (*it).numSectors() * bc_.sectorSize();
            }
            int numBlocks = (sizeToRead2 + MAX_CONTIGOUS_SIZE - 1) / MAX_CONTIGOUS_SIZE;
//            logprintf("%s %s:%d numSectors=%d sizeToRead2=%d numBlocks==%d\n", __func__, __FILE__, __LINE__, (*it).numSectors(), sizeToRead2, numBlocks);
            int restToRead = sizeToRead2;
            for (int i = 0; i < numBlocks; i++) {
                int size = restToRead > MAX_CONTIGOUS_SIZE ? MAX_CONTIGOUS_SIZE : restToRead;
                // logprintf("%s %s:%d restToRead=%d size=%d\n", __func__, __FILE__, __LINE__, restToRead, size);
                // logprintf("[readInternal] bufidx=%d sector=%d size=%d %s %s:%d\n",
                //           (int)(((*it).startSector() - sector) + i * MAX_CONTIGOUS_SIZE),
                //           (int)((*it).startSector() + (MAX_CONTIGOUS_SIZE / getSectorSize()) * i),
                //           (int)size, __func__, __FILE__, __LINE__);
//                logprintf("read readBuf=%x i=%d ((*it).startSector() - sector)= %d (*it).startSector()= %d(((*it).startSector() - sector) + i * MAX_CONTIGOUS_SIZE)=%x\n", readBuf, i, (int)(((*it).startSector() - sector)), (int)((*it).startSector()), (int)(((*it).startSector() - sector) + i * MAX_CONTIGOUS_SIZE));
                int ret = readInternal(((uint8_t*)readBuf) + (((*it).startSector() - sector) * getSectorSize() + i * MAX_CONTIGOUS_SIZE),
                                       (*it).startSector() + (MAX_CONTIGOUS_SIZE / getSectorSize()) * i
                                       , size);
//        logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
                if (ret < 0) {
                    return ret;
                }
                restToRead -= size;
            }
            ASSERT(restToRead == 0);
        }
        // logprintf("returned %d %s %s:%d\n", (int)sizeToRead, __func__, __FILE__, __LINE__);
        // logprintf("\n\n**************** RETURN");
        // for (int i = 0; i < (int)sizeToRead; i++) {
        //     logprintf("%c", ((uint8_t*)readBuf)[i]);
        // }
        // logprintf("\n\n");
        return sizeToRead;
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

    void waitWithBusyLoop(uintptr_t usec)
    {
        for (uintptr_t i = 0; i < usec; i++) {
            delayMicrosec();
            if (vq_->isUsedBufExist()) {
                break;
            }
        }
    }

private:
    void memoryBarrier()
    {

    }

    int64_t readInternal(void* readBuf, int64_t sector, int64_t sizeToRead)
    {
//        logprintf("readBuf=%x\n", readBuf);
        // Possible enhancement
        //   For now, we allocate ContigousMemory for each time, we can elminate allocating buffer.
        //   readBuf can be used directory using scatter gather system.

        // seems to less than 512 byte can't be read.
        int64_t adjSizeToRead = ((sizeToRead + getSectorSize() - 1) / getSectorSize()) * getSectorSize();

        scoped_ptr<ContigousMemory> mem(ContigousMemory::allocate(adjSizeToRead + sizeof(virtio_blk_outhdr) + 1));
        if (mem.get() == NULL) {
            logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
            return M_NO_MEMORY;
        }
        struct virtio_blk_outhdr* hdr = (struct virtio_blk_outhdr*)mem->get();

        std::vector<VirtBuffer> out;
        hdr->type = VIRTIO_BLK_T_IN ;
        hdr->ioprio = 0;
        hdr->sector = sector;
        out.push_back(VirtBuffer(hdr, sizeof(struct virtio_blk_outhdr)));

        std::vector<VirtBuffer> in;

        // We need volatile keyword here, since status will be updated by host.
        volatile uint8_t* status = (uint8_t*)((uintptr_t)mem->get() + sizeof(struct virtio_blk_outhdr));
        *status = 0xff;

        uint8_t* buf = (uint8_t*)((uintptr_t)mem->get() + sizeof(struct virtio_blk_outhdr) + 1);
        memset(buf, 0, adjSizeToRead);
        in.push_back(VirtBuffer(buf, adjSizeToRead));

        in.push_back(VirtBuffer(status, 1));

        static uintptr_t cookie = 1;
        cookie++;
        intptr_t addBufRet = vq_->addBuf(out, in, (void*)cookie);
        if (addBufRet != M_OK) {
            monapi_warn("addBuf failed %d", addBufRet);
            return addBufRet;
        }
        vq_->kick();

        waitWithBusyLoop(2000);

        // gave up busy loop, use expensive waitInterrupt.
        while (!vq_->isUsedBufExist()) {
            vdev_->waitInterrupt();
        }

        memoryBarrier();

        int sizeRead = 0;
        void* afterCookie = vq_->getBuf(sizeRead);

        sizeRead -= sizeof(*status);

        // In theory, this busy loop is not necessary.
        // But on this driver, sometime after getBut is called, the status is not updated.
        // This may be a bug of the driver.
        while (*status == 0xff) {
//            _logprintf("waiting");
        }
        if (*status != VIRTIO_BLK_S_OK) {
            monapi_warn("getBuf failed %d:%d", (int)(*status), (*status != VIRTIO_BLK_S_OK));
            return M_READ_ERROR;
        }

        if (sizeRead == adjSizeToRead) {
            MonAPI::scoped_ptr<uint8_t> p(new uint8_t[adjSizeToRead]);
            ASSERT(p.get());
            memcpy(p.get(), buf, adjSizeToRead);
//            logprintf("dest=%x %s %s:%d\n", p.get(), __func__, __FILE__, __LINE__);

            // logprintf("cached by read sector=%d - %d\n", (int)sector, (int)sector + adjSizeToRead / 512);
            // for (int i = 0; i < adjSizeToRead; i++) {
            //     logprintf("%c", buf[i]);
            // }
            // logprintf("\n\n");

            bc_.addRange(sector, adjSizeToRead / 512, p.get());
        }

        ASSERT((uintptr_t)afterCookie == cookie);
        ASSERT(sizeRead <= adjSizeToRead);
        memcpy(readBuf, buf, sizeToRead);
//        logprintf("dest=%x %s %s:%d\n", readBuf, __func__, __FILE__, __LINE__);
        return sizeRead >= sizeToRead ? sizeToRead : sizeRead;
    }

    MonAPI::scoped_ptr<VirtioDevice> vdev_;
    MonAPI::scoped_ptr<VirtQueue> vq_;
    BlockCache bc_;
};

#endif // _VIRTIOBLOCK_
