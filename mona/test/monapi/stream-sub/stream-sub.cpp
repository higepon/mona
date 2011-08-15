#include <monapi.h>

#define MUNIT_GLOBAL_VALUE_DEFINED
#include <monapi/MUnit.h>
#include <stdlib.h>

using namespace MonAPI;

#include "../stream/test_data.c"

static void processForTestOneProcessWritesTheOtherProcessReadsExactSameData(uintptr_t streamHandle)
{
    scoped_ptr<Stream> s(Stream::createFromHandle(streamHandle));
    uint8_t buf[DATA_SIZE];
    EXPECT_EQ(DATA_SIZE, s->read(buf, DATA_SIZE));
    EXPECT_EQ(0, memcmp(TEST_DATA, buf, DATA_SIZE));
}

static void processForTestOneProcessFillsStreamTheOtherProcessTryToWriteReturnsZero(uintptr_t streamHandle)
{
    scoped_ptr<Stream> s(Stream::createFromHandle(streamHandle));
    ASSERT_EQ(0, s->write(TEST_DATA, DATA_SIZE));
}

static void processForTestOneProcessWaitsToReadTheOtherProcessWritesSomeAndNotifiesToTheWaitingProcess(uintptr_t streamHandle)
{
    scoped_ptr<Stream> s(Stream::createFromHandle(streamHandle));
    ASSERT_EQ(3, s->write(TEST_DATA, DATA_SIZE));
}

static void processForTestOneProcessWritesToFullStreamImmediatlyReturnsZero(uintptr_t streamHandle)
{
    scoped_ptr<Stream> s(Stream::createFromHandle(streamHandle));
    ASSERT_EQ(0, s->write(TEST_DATA, DATA_SIZE));
}

static void processForTestOneProcessWaitsToWriteTheOtherProcessReadsSomeAndNotifiesToTheWaitingProcess(uintptr_t streamHandle)
{
    scoped_ptr<Stream> s(Stream::createFromHandle(streamHandle));
    uint8_t buf[SMALL_STREAM_SIZE_BYTE];
    EXPECT_EQ(DATA_SIZE, s->read(buf, SMALL_STREAM_SIZE_BYTE));
    EXPECT_EQ(0, memcmp(TEST_DATA, buf, SMALL_STREAM_SIZE_BYTE));
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        MONA_ASSERT(false);
        return -1;
    }
    uintptr_t streamHandle = strtol(argv[2], NULL, 16);
    if (strcmp(argv[1], "testOneProcessWritesTheOtherProcessReadsExactSameData") == 0) {
        processForTestOneProcessWritesTheOtherProcessReadsExactSameData(streamHandle);
    } else if (strcmp(argv[1], "testOneProcessFillsStreamTheOtherProcessTryToWriteReturnsZero") == 0) {
        processForTestOneProcessFillsStreamTheOtherProcessTryToWriteReturnsZero(streamHandle);
    } else if (strcmp(argv[1], "testOneProcessWaitsToReadTheOtherProcessWritesSomeAndNotifiesToTheWaitingProcess") == 0) {
        processForTestOneProcessWaitsToReadTheOtherProcessWritesSomeAndNotifiesToTheWaitingProcess(streamHandle);
    } else if (strcmp(argv[1], "testOneProcessWritesToFullStreamImmediatlyReturnsZero") == 0) {
        processForTestOneProcessWritesToFullStreamImmediatlyReturnsZero(streamHandle);
    } else if (strcmp(argv[1], "testOneProcessWaitsToWriteTheOtherProcessReadsSomeAndNotifiesToTheWaitingProcess") == 0) {
        processForTestOneProcessWaitsToWriteTheOtherProcessReadsSomeAndNotifiesToTheWaitingProcess(streamHandle);
    } else {
        MONA_ASSERT(false);
        return -1;
    }
    TEST_RESULTS();
    return 0;
}
