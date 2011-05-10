#include <monapi.h>
#define MUNIT_GLOBAL_VALUE_DEFINED
#include <monapi/MUnit.h>
#include <drivers/BlockCache.h>

#define EXPECT_IO_REQUEST_EQ(lhs, rhs) {             \
    EXPECT_EQ(lhs.startSector(), rhs.startSector()); \
    EXPECT_EQ(lhs.numSectors(), rhs.numSectors());   \
}

#define EXPECT_CACHE_EQ(lhs, rhs) {        \
    EXPECT_EQ(lhs.sector(), rhs.sector()); \
    EXPECT_TRUE(memcmp(lhs.get(), rhs.get(), BlockCache::sectorSize()) == 0); \
}

static const int MAX_NUM_CACHES = 10;

static void testEmptyCacheHasNoCacheOf0thSector()
{
    BlockCache bc(MAX_NUM_CACHES);
    Cache cache;
    const int startSector = 0;
    const int numSectors = 1;
    EXPECT_EQ(false, bc.get(startSector, numSectors, cache));
}

static void testAddedSingleCacheCanGet()
{
    BlockCache bc(MAX_NUM_CACHES);
    const int targetSector = 0;
    Cache foundCache;
    const int numSectors = 1;
    Cache cache(targetSector, new uint8_t[3]);
    EXPECT_FALSE(bc.add(cache));
    EXPECT_EQ(true, bc.get(targetSector, numSectors, foundCache));
    EXPECT_CACHE_EQ(cache, foundCache);
}

static void testAddedMultipleCacheCanGetSingleCache()
{
    BlockCache bc(MAX_NUM_CACHES);
    EXPECT_FALSE(bc.add(Cache(0, new uint8_t[3])));
    Cache target(1, new uint8_t[3]);
    EXPECT_FALSE(bc.add(target));
    Cache cache;
    EXPECT_EQ(true, bc.get(1, 1, cache));
    EXPECT_CACHE_EQ(target, cache);
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
    Cache cache;
    // The sector 1 is recently used.
    ASSERT_EQ(true, bc.get(1, 1, cache));

    uint8_t* r = new uint8_t[bc.sectorSize()];
    EXPECT_FALSE(bc.add(Cache(2, r)));
    // The sector 0 is destroyed.
    EXPECT_FALSE(bc.get(0, 1, cache));
    EXPECT_TRUE(bc.get(1, 1, cache));
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
