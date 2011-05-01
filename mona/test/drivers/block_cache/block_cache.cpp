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

typedef std::vector<Cache> CacheList;
typedef std::vector<IORequest> IORequests;
typedef std::map<uintptr_t, Cache> CacheMap;

class BlockCache
{
public:
    BlockCache(uintptr_t maxCacheSizeByte) : maxCacheSizeByte_(maxCacheSizeByte)
    {
    }

    bool get(uintptr_t startSector, uintptr_t numSectors, CacheList& cacheList)
    {
        CacheMap::iterator it = cacheMap_.find(startSector);
        if (it == cacheMap_.end()) {
            return false;
        } else {
            cacheList.push_back((*it).second);
            return true;
        }
    }

    bool getCacheAndRest(uintptr_t startSector, uintptr_t numSectors, CacheList& cacheList, IORequests& rest)
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
                } else if ((*it).first > sector) {
                    rest.push_back(IORequest(sector, 1));
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

private:
    CacheMap cacheMap_;
    uintptr_t maxCacheSizeByte_;
};

#define EXPECT_CACHE_EQ(lhs, rhs) {        \
    EXPECT_EQ(lhs.sector(), rhs.sector()); \
    EXPECT_EQ(lhs.get(), rhs.get());       \
}

static const int MAX_CACHE_SIZE = 1 * 1024 * 1024;

static void testEmptyCacheHasNoCacheOf0thSector()
{
    BlockCache bc(MAX_CACHE_SIZE);
    CacheList cacheList;
    const int startSector = 0;
    const int numSectors = 1;
    EXPECT_EQ(false, bc.get(startSector, numSectors, cacheList));
    EXPECT_EQ(0, cacheList.size());
}

static void testAddedSingleCacheCanGet()
{
    BlockCache bc(MAX_CACHE_SIZE);
    const int targetSector = 0;
    CacheList cacheList;
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
    CacheList cacheList;
    EXPECT_EQ(true, bc.get(1, 1, cacheList));
    ASSERT_EQ(1, cacheList.size());
    EXPECT_CACHE_EQ(target, cacheList[0]);
}

static void testFoundPartialCacheAndRestToRead()
{
    BlockCache bc(MAX_CACHE_SIZE);
    Cache target(1, (void*)0xdeadbeaf);
    EXPECT_TRUE(bc.add(target));
    CacheList cacheList;
    IORequests rest;
    EXPECT_EQ(true, bc.getCacheAndRest(0, 2, cacheList, rest));
    ASSERT_EQ(1, cacheList.size());
    EXPECT_CACHE_EQ(target, cacheList[0]);
    ASSERT_EQ(1, rest.size());
    EXPECT_EQ(0, rest[0].startSector());
    EXPECT_EQ(1, rest[0].numSectors());

}

// todo
// const_iterator
int main(int argc, char *argv[])
{
    testEmptyCacheHasNoCacheOf0thSector();
    testAddedSingleCacheCanGet();
    testAddedMultipleCacheCanGetSingleCache();
    testFoundPartialCacheAndRestToRead();
    TEST_RESULTS();
    return 0;
}
