/*
 *   Copyright (c) 2011  Higepon(Taro Minowa)  <higepon@users.sourceforge.jp>
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

#ifndef _CACHED_BLOCK_DEVICE_
#define _CACHED_BLOCK_DEVICE_

#include <drivers/BlockDevice.h>
#include <drivers/BlockCache.h>

class CachedBlockDevice : public BlockDevice
{
public:
    CachedBlockDevice(BlockDevice* dev) : dev_(dev), bc_(60000)
    {
        ASSERT(dev_);
    }

    CachedBlockDevice(BlockDevice* dev, uintptr_t maxNumSectors) : dev_(dev), bc_(maxNumSectors)
    {
        ASSERT(dev_);
    }
    int64_t write(const void* writeBuf, int64_t sector, int64_t sizeToWrite)
    {
        int64_t sizeWritten = dev_->write(writeBuf, sector, sizeToWrite);
        if (sizeWritten < 0) {
            return sizeWritten;
        } else {
            bc_.addRange(sector, sizeWritten / sectorSize(), writeBuf);
            return sizeWritten;
        }
    }

    int64_t read(void* readBuf, int64_t sector, int64_t sizeToRead)
    {
        uintptr_t numSectors = (sizeToRead - 1 + sectorSize()) / sectorSize();
        Caches caches;
        IORequests requests;
        bc_.getCacheAndRest(sector, numSectors, caches, requests);
        readFromCache(readBuf, caches, sizeToRead, sector, numSectors);
        for (IORequests::const_iterator it = requests.begin(); it != requests.end(); ++it) {
            const IORequest& req = (*it);
            int64_t requestSizeToRead = calcRequestSizeByte(req, sector, numSectors, sizeToRead);
            uint8_t* dest = (uint8_t*)readBuf + (req.startSector() - sector) * sectorSize();
            int64_t ret = dev_->read(dest, req.startSector(), requestSizeToRead);
            if (ret < 0) {
                return ret;
            }
            ASSERT(ret == requestSizeToRead);
            MonAPI::scoped_array<uint8_t> p(new uint8_t[requestSizeToRead]);
            ASSERT(p.get());
            memcpy(p.get(), dest, requestSizeToRead);
            bc_.addRange(req.startSector(), requestSizeToRead / sectorSize(), p.get());

        }
        return sizeToRead;
    }

    uint64_t sectorSize() const
    {
        return dev_->sectorSize();
    }

    uint64_t getCapacity() const
    {
        return dev_->getCapacity();
    }
private:

    void readFromCache(void* readBuf, const Caches& caches, int64_t sizeToRead, int64_t sector, uintptr_t numSectors)
    {
        bool isExactSectorSize = (sizeToRead % sectorSize())== 0;
        for (Caches::const_iterator it = caches.begin(); it != caches.end(); ++it) {
            bool isLastSector = (*it).sector() == (sector + numSectors - 1);
            int copySize = 0;
            if (isLastSector && !isExactSectorSize) {
                copySize = sizeToRead % sectorSize();
            } else {
                copySize = sectorSize();
            }
            memcpy((uint8_t*)readBuf + sectorSize() * ((*it).sector() - sector), (*it).get(), copySize);
        }
    }

    int64_t calcRequestSizeByte(const IORequest& req, int64_t sector, uintptr_t numSectors, int64_t sizeToRead) const
    {
        bool includesLastSector = (req.startSector() + req.numSectors() == sector + numSectors);
        if (includesLastSector) {
            return (req.numSectors() - 1) * sectorSize() + ((int)sizeToRead % sectorSize() == 0 ? sectorSize() : (int)sizeToRead % sectorSize());
        } else {
            return req.numSectors() * sectorSize();
        }
    }

    BlockDevice* dev_;
    BlockCache bc_;
};

#endif // _CACHED_BLOCK_DEVICE_
