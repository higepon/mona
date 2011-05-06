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

#ifndef _BLOCK_CACHE_
#define _BLOCK_CACHE_

#include <vector>
#include <map>

using namespace std;

class IORequest
{
public:
    IORequest(uintptr_t startSector, uintptr_t numSectors) :
        startSector_(startSector),
        numSectors_(numSectors)
    {
    }

    uintptr_t startSector() const
    {
        return startSector_;
    }

    void incrementNumSectors()
    {
        numSectors_++;
    }

    uintptr_t numSectors() const
    {
        return numSectors_;
    }

private:
    uintptr_t startSector_;
    uintptr_t numSectors_;
};

class Cache
{
public:
    Cache() {}
    Cache(uintptr_t sector, void* data) :
        sector_(sector),
        data_(data),
        isRecentlyUsed_(false)
    {

    }

    void destroy()
    {
        delete[] (uint8_t*)data_;
    }

    uintptr_t sector() const
    {
        return sector_;
    }

    void* get() const
    {
        return data_;
    }

    bool isRecentlyUsed() const
    {
        return isRecentlyUsed_;
    }

    void setAsNotUsed()
    {
        isRecentlyUsed_ = false;
    }

    void setAsUsed()
    {
        isRecentlyUsed_ = true;
    }


private:
    uintptr_t sector_;
    void* data_;
    bool isRecentlyUsed_;
};

typedef std::vector<Cache> Caches;
typedef std::vector<IORequest> IORequests;
typedef std::map<uintptr_t, Cache> CacheMap;

class BlockCache
{
public:
    BlockCache(uintptr_t maxNumCaches) : maxNumCaches_(maxNumCaches)
    {
    }

    virtual ~BlockCache()
    {
        for (CacheMap::iterator it = cacheMap_.begin(); it != cacheMap_.end(); ++it) {
            (*it).second.destroy();
        }
    }

    bool get(uintptr_t startSector, uintptr_t numSectors, Cache& ret)
    {
        CacheMap::iterator it = cacheMap_.find(startSector);
        if (it == cacheMap_.end()) {
            return false;
        } else {
            (*it).second.setAsUsed();
            ret = (*it).second;
            return true;
        }
    }

    uintptr_t sectorSize() const
    {
        return 512;
    }

    bool getCacheAndRest(uintptr_t startSector, uintptr_t numSectors, Caches& cacheList, IORequests& rest)
    {
        CacheMap::const_iterator it = cacheMap_.lower_bound(startSector);
        for (uintptr_t sector = startSector; sector < startSector + numSectors; sector++) {
            if (it == cacheMap_.end()) {
                uintptr_t restNumSectors = numSectors - sector + startSector;
                rest.push_back(IORequest(sector, restNumSectors));
                break;
            } else {
                if ((*it).first == sector) {
                    cacheList.push_back((*it).second);
                    ++it;
                } else if ((*it).first > sector) {
                    if (!rest.empty()) {
                        IORequest& last = rest[rest.size() - 1];
                        bool isNeighbor = sector == last.startSector() + last.numSectors();
                        if (isNeighbor) {
                            last.incrementNumSectors();
                        } else {
                            rest.push_back(IORequest(sector, 1));
                        }
                    } else {
                        rest.push_back(IORequest(sector, 1));
                    }
                } else {
                    ++it;
                }
            }
        }
        return true;
    }

    bool add(Cache cache)
    {
        if (isCacheFull()) {
            invalidateNotRecentlyUsed();
        }
        ASSERT(!isCacheFull());
        CacheMap::iterator it = cacheMap_.find(cache.sector());
        if (it == cacheMap_.end()) {
            cacheMap_[cache.sector()] = cache;
            return false;
        } else {
            // overwrite
            (*it).second.destroy();
            cacheMap_[cache.sector()] = cache;
//            (*it).second = cache;
            return true;
        }
    }

    bool addRange(uintptr_t startSector, uintptr_t numSectors, const void* data)
    {
        for (uintptr_t i = 0; i < numSectors; i++) {
            uint8_t* p = new uint8_t[sectorSize()];
            memcpy(p, ((uint8_t*)data) + i * sectorSize(), sectorSize());
            logprintf("cached sector=%d address=%x\n", startSector + i, p);
            Cache cache(startSector + i, p);
            add(cache);
        }
        return true;
    }

private:

    void invalidateNotRecentlyUsed()
    {
        for (CacheMap::iterator it = cacheMap_.begin(); it != cacheMap_.end(); ) {
            Cache& c = (*it).second;
            if (c.isRecentlyUsed()) {
                c.setAsNotUsed();
                it++;
            } else {
                c.destroy();
                cacheMap_.erase(it++);
            }
        }
        // A rare case, all cache is recently used.
        if (isCacheFull()) {
            for (CacheMap::iterator it = cacheMap_.begin(); it != cacheMap_.end(); ) {
                Cache& c = (*it).second;
                c.destroy();
                cacheMap_.erase(it++);
            }
        }
    }

    bool isCacheFull() const
    {
        return cacheMap_.size() == maxNumCaches_;
    }

    CacheMap cacheMap_;
    uintptr_t maxNumCaches_;
};

#endif // _BLOCK_CACHE_
