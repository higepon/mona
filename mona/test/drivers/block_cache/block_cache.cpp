#include <monapi.h>
#define MUNIT_GLOBAL_VALUE_DEFINED
#include <monapi/MUnit.h>
#include <vector>
#include <map>

using namespace std;

class Cache
{
public:
    Cache() {}
    Cache(uintptr_t sector, void* cache) {}

    uintptr_t sector() const
    {
        return 0;
    }

    void* get() const
    {
        return NULL;
    }
};

typedef std::vector<Cache> CacheList;
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

    bool add(uintptr_t sector, void* cache)
    {
        cacheMap_[sector] = Cache(sector, cache);
        return true;
    }

private:
    CacheMap cacheMap_;
    uintptr_t maxCacheSizeByte_;
};

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

// todo overwrite cache
// todo match sector
// operator =
// single, multiple
static void testAddedSingleCacheCanGet()
{
    BlockCache bc(MAX_CACHE_SIZE);
    const int targetSector = 0;
    CacheList cacheList;
    const int numSectors = 1;
    EXPECT_TRUE(bc.add(targetSector, NULL));
    EXPECT_EQ(true, bc.get(targetSector, numSectors, cacheList));
    ASSERT_EQ(1, cacheList.size());
    Cache foundCache = cacheList[0];
    EXPECT_EQ(0, foundCache.sector());
    EXPECT_EQ(NULL, foundCache.get());
}

int main(int argc, char *argv[])
{
    testEmptyCacheHasNoCacheOf0thSector();
    testAddedSingleCacheCanGet();
    TEST_RESULTS();
    return 0;
}
