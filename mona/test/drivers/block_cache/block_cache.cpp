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
        data_(data)
    {

    }

    uintptr_t sector() const
    {
        return sector_;
    }

    void* get() const
    {
        return data_;
    }

private:
    uintptr_t sector_;
    void* data_;
};

typedef std::vector<Cache> Caches;
typedef std::vector<IORequest> IORequests;
typedef std::map<uintptr_t, Cache> CacheMap;

class BlockCache
{
public:
    BlockCache(uintptr_t maxCacheSizeByte) : maxCacheSizeByte_(maxCacheSizeByte)
    {
    }

    virtual ~BlockCache() {}

    bool get(uintptr_t startSector, uintptr_t numSectors, Caches& cacheList)
    {
        CacheMap::const_iterator it = cacheMap_.find(startSector);
        if (it == cacheMap_.end()) {
            return false;
        } else {
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
        cacheMap_[cache.sector()] = cache;
        return true;
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
    CacheMap cacheMap_;
    uintptr_t maxCacheSizeByte_;
};

#define EXPECT_IO_REQUEST_EQ(lhs, rhs) {             \
    EXPECT_EQ(lhs.startSector(), rhs.startSector()); \
    EXPECT_EQ(lhs.numSectors(), rhs.numSectors());   \
}

#define EXPECT_CACHE_EQ(lhs, rhs) {        \
    EXPECT_EQ(lhs.sector(), rhs.sector()); \
    EXPECT_EQ(lhs.get(), rhs.get());       \
}

static const int MAX_CACHE_SIZE = 1 * 1024 * 1024;

static void testEmptyCacheHasNoCacheOf0thSector()
{
    BlockCache bc(MAX_CACHE_SIZE);
    Caches cacheList;
    const int startSector = 0;
    const int numSectors = 1;
    EXPECT_EQ(false, bc.get(startSector, numSectors, cacheList));
    EXPECT_EQ(0, cacheList.size());
}

static void testAddedSingleCacheCanGet()
{
    BlockCache bc(MAX_CACHE_SIZE);
    const int targetSector = 0;
    Caches cacheList;
    const int numSectors = 1;
    Cache cache(targetSector, NULL);
    EXPECT_TRUE(bc.add(cache));
    EXPECT_EQ(true, bc.get(targetSector, numSectors, cacheList));
    ASSERT_EQ(1, cacheList.size());
    EXPECT_CACHE_EQ(cache, cacheList[0]);
}

static void testAddedMultipleCacheCanGetSingleCache()
{
    BlockCache bc(MAX_CACHE_SIZE);
    EXPECT_TRUE(bc.add(Cache(0, (void*)0x12345678)));
    Cache target(1, (void*)0xdeadbeaf);
    EXPECT_TRUE(bc.add(target));
    Caches cacheList;
    EXPECT_EQ(true, bc.get(1, 1, cacheList));
    ASSERT_EQ(1, cacheList.size());
    EXPECT_CACHE_EQ(target, cacheList[0]);
}

static void testFoundPartialCacheAndRestToRead()
{
    BlockCache bc(MAX_CACHE_SIZE);
    Cache target(1, (void*)0xdeadbeaf);
    EXPECT_TRUE(bc.add(target));
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
    BlockCache bc(MAX_CACHE_SIZE);
    EXPECT_TRUE(bc.add(Cache(0, (void*)0xdeadbeaf)));
    Caches cacheList;
    IORequests rest;
    EXPECT_EQ(true, bc.getCacheAndRest(0, 3, cacheList, rest));
    ASSERT_EQ(1, cacheList.size());
    ASSERT_EQ(1, rest.size());
    EXPECT_IO_REQUEST_EQ(IORequest(1, 2), rest[0]);
}

static void testHeadOfRestShouldBeMergedAsFarAsPossible()
{
    BlockCache bc(MAX_CACHE_SIZE);
    EXPECT_TRUE(bc.add(Cache(2, (void*)0xdeadbeaf)));
    Caches cacheList;
    IORequests rest;
    EXPECT_EQ(true, bc.getCacheAndRest(0, 3, cacheList, rest));
    ASSERT_EQ(1, cacheList.size());
    ASSERT_EQ(1, rest.size());
    EXPECT_IO_REQUEST_EQ(IORequest(0, 2), rest[0]);
}

static void testMiddleOfRestShouldBeMergedAsFarAsPossible()
{
    BlockCache bc(MAX_CACHE_SIZE);
    EXPECT_TRUE(bc.add(Cache(0, (void*)0xdeadbeaf)));
    EXPECT_TRUE(bc.add(Cache(3, (void*)0xdeadbeaf)));
    Caches cacheList;
    IORequests rest;
    EXPECT_EQ(true, bc.getCacheAndRest(0, 4, cacheList, rest));
    ASSERT_EQ(2, cacheList.size());
    ASSERT_EQ(1, rest.size());
    EXPECT_IO_REQUEST_EQ(IORequest(1, 2), rest[0]);
}

static void testHandleRangeCacheAdded()
{
    BlockCache bc(MAX_CACHE_SIZE);
    uintptr_t cacheStartAddress = 0x1000000;
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

// overwrite when exists
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
    TEST_RESULTS();
    return 0;
}
