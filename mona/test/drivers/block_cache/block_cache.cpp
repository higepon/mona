#include <monapi.h>
#define MUNIT_GLOBAL_VALUE_DEFINED
#include <monapi/MUnit.h>
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

    bool get(uintptr_t startSector, uintptr_t numSectors, Caches& cacheList)
    {
        CacheMap::iterator it = cacheMap_.find(startSector);
        if (it == cacheMap_.end()) {
            return false;
        } else {
            (*it).second.setAsUsed();
            cacheList.push_back((*it).second);
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
            (*it).second = cache;
            return true;
        }
    }

    bool addRange(uintptr_t startSector, uintptr_t numSectors, void* data)
    {
        for (uintptr_t i = 0; i < numSectors; i++) {
            Cache cache(startSector + i, (void*)((uintptr_t)data + i * sectorSize()));
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
    }

    bool isCacheFull() const
    {
        return cacheMap_.size() == maxNumCaches_;
    }

    CacheMap cacheMap_;
    uintptr_t maxNumCaches_;
};

#define EXPECT_IO_REQUEST_EQ(lhs, rhs) {             \
    EXPECT_EQ(lhs.startSector(), rhs.startSector()); \
    EXPECT_EQ(lhs.numSectors(), rhs.numSectors());   \
}

#define EXPECT_CACHE_EQ(lhs, rhs) {        \
    EXPECT_EQ(lhs.sector(), rhs.sector()); \
    EXPECT_EQ(lhs.get(), rhs.get());       \
}

static const int MAX_NUM_CACHES = 10;

static void testEmptyCacheHasNoCacheOf0thSector()
{
    BlockCache bc(MAX_NUM_CACHES);
    Caches cacheList;
    const int startSector = 0;
    const int numSectors = 1;
    EXPECT_EQ(false, bc.get(startSector, numSectors, cacheList));
    EXPECT_EQ(0, cacheList.size());
}

static void testAddedSingleCacheCanGet()
{
    BlockCache bc(MAX_NUM_CACHES);
    const int targetSector = 0;
    Caches cacheList;
    const int numSectors = 1;
    Cache cache(targetSector, new uint8_t[3]);
    EXPECT_FALSE(bc.add(cache));
    EXPECT_EQ(true, bc.get(targetSector, numSectors, cacheList));
    ASSERT_EQ(1, cacheList.size());
    EXPECT_CACHE_EQ(cache, cacheList[0]);
}

static void testAddedMultipleCacheCanGetSingleCache()
{
    BlockCache bc(MAX_NUM_CACHES);
    EXPECT_FALSE(bc.add(Cache(0, new uint8_t[3])));
    Cache target(1, new uint8_t[3]);
    EXPECT_FALSE(bc.add(target));
    Caches cacheList;
    EXPECT_EQ(true, bc.get(1, 1, cacheList));
    ASSERT_EQ(1, cacheList.size());
    EXPECT_CACHE_EQ(target, cacheList[0]);
}

static void testFoundPartialCacheAndRestToRead()
{
    BlockCache bc(MAX_NUM_CACHES);
    Cache target(1, (void*)0xdeadbeaf);
    EXPECT_FALSE(bc.add(target));
    Caches cacheList;
    IORequests rest;
    EXPECT_EQ(true, bc.getCacheAndRest(0, 2, cacheList, rest));
    ASSERT_EQ(1, cacheList.size());
    EXPECT_CACHE_EQ(target, cacheList[0]);
    ASSERT_EQ(1, rest.size());
    EXPECT_IO_REQUEST_EQ(IORequest(0, 1), rest[0]);
}

static void testTailOfRestShouldBeMergedAsFarAsPossible()
{
    BlockCache bc(MAX_NUM_CACHES);
    EXPECT_FALSE(bc.add(Cache(0, new uint8_t[3])));
    Caches cacheList;
    IORequests rest;
    EXPECT_EQ(true, bc.getCacheAndRest(0, 3, cacheList, rest));
    ASSERT_EQ(1, cacheList.size());
    ASSERT_EQ(1, rest.size());
    EXPECT_IO_REQUEST_EQ(IORequest(1, 2), rest[0]);
}

static void testHeadOfRestShouldBeMergedAsFarAsPossible()
{
    BlockCache bc(MAX_NUM_CACHES);
    EXPECT_FALSE(bc.add(Cache(2, new uint8_t[3])));
    Caches cacheList;
    IORequests rest;
    EXPECT_EQ(true, bc.getCacheAndRest(0, 3, cacheList, rest));
    ASSERT_EQ(1, cacheList.size());
    ASSERT_EQ(1, rest.size());
    EXPECT_IO_REQUEST_EQ(IORequest(0, 2), rest[0]);
}

static void testMiddleOfRestShouldBeMergedAsFarAsPossible()
{
    BlockCache bc(MAX_NUM_CACHES);
    EXPECT_FALSE(bc.add(Cache(0, new uint8_t[3])));
    EXPECT_FALSE(bc.add(Cache(3, new uint8_t[3])));
    Caches cacheList;
    IORequests rest;
    EXPECT_EQ(true, bc.getCacheAndRest(0, 4, cacheList, rest));
    ASSERT_EQ(2, cacheList.size());
    ASSERT_EQ(1, rest.size());
    EXPECT_IO_REQUEST_EQ(IORequest(1, 2), rest[0]);
}

static void testHandleRangeCacheAdded()
{
    BlockCache bc(MAX_NUM_CACHES);
    uint8_t* cacheStartAddress = new uint8_t[bc.sectorSize() * 2];
    EXPECT_TRUE(bc.addRange(1, 2, (void*)cacheStartAddress));
    Caches cacheList;
    IORequests rest;
    EXPECT_EQ(true, bc.getCacheAndRest(0, 4, cacheList, rest));
    ASSERT_EQ(2, cacheList.size());
    EXPECT_CACHE_EQ(Cache(1, (void*)cacheStartAddress), cacheList[0]);
    EXPECT_CACHE_EQ(Cache(2, (void*)(cacheStartAddress + bc.sectorSize())), cacheList[1]);
    ASSERT_EQ(2, rest.size());
    EXPECT_IO_REQUEST_EQ(IORequest(0, 1), rest[0]);
    EXPECT_IO_REQUEST_EQ(IORequest(3, 1), rest[1]);
}

static void testCacheProperyDestroyedWhenUpdated()
{
    BlockCache bc(MAX_NUM_CACHES);
    uint8_t* p = new uint8_t[bc.sectorSize()];
    uint8_t* q = new uint8_t[bc.sectorSize()];
    EXPECT_FALSE(bc.add(Cache(0, p)));
    EXPECT_TRUE(bc.add(Cache(0, q)));
}

static void testNotRecentlyUsedCacheShouldBeDestroyedWhenFull()
{
    const int maxNumCaches = 2;
    BlockCache bc(maxNumCaches);
    uint8_t* p = new uint8_t[bc.sectorSize()];
    uint8_t* q = new uint8_t[bc.sectorSize()];
    EXPECT_FALSE(bc.add(Cache(0, p)));
    EXPECT_FALSE(bc.add(Cache(1, q)));
    Caches cacheList;
    // The sector 1 is recently used.
    ASSERT_EQ(true, bc.get(1, 1, cacheList));

    uint8_t* r = new uint8_t[bc.sectorSize()];
    EXPECT_FALSE(bc.add(Cache(2, r)));
    // The sector 0 is destroyed.
    EXPECT_FALSE(bc.get(0, 1, cacheList));
    EXPECT_TRUE(bc.get(1, 1, cacheList));
}

// handle max size
int main(int argc, char *argv[])
{
    testEmptyCacheHasNoCacheOf0thSector();
    testAddedSingleCacheCanGet();
    testAddedMultipleCacheCanGetSingleCache();
    testFoundPartialCacheAndRestToRead();
    testTailOfRestShouldBeMergedAsFarAsPossible();
    testHeadOfRestShouldBeMergedAsFarAsPossible();
    testMiddleOfRestShouldBeMergedAsFarAsPossible();
    testHandleRangeCacheAdded();
    testCacheProperyDestroyedWhenUpdated();
    testNotRecentlyUsedCacheShouldBeDestroyedWhenFull();
    TEST_RESULTS();
    return 0;
}
