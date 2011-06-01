#include <monapi.h>

#define MUNIT_GLOBAL_VALUE_DEFINED
#include <monapi/MUnit.h>
#include <stdlib.h>

using namespace MonAPI;

#include "../stream/test_data.c"

static void processForTestOneProcessWritesTheOtherProcessReadsExactSameData(uintptr_t streamHandle)
{
    scoped_ptr<Stream> s(Stream::FromHandle(streamHandle));
    uint8_t buf[DATA_SIZE];
    EXPECT_EQ(DATA_SIZE, s->read(buf, DATA_SIZE));
    EXPECT_EQ(0, memcmp(TEST_DATA, buf, DATA_SIZE));
}

static void processForTestOneProcessFillsStreamTheOtherProcessTryToWriteReturnsZero(uintptr_t streamHandle)
{
    scoped_ptr<Stream> s(Stream::FromHandle(streamHandle));
    ASSERT_EQ(0, s->write(TEST_DATA, DATA_SIZE));
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        return -1;
    }
    uintptr_t streamHandle = strtol(argv[2], NULL, 16);
    if (strcmp(argv[1], "testOneProcessWritesTheOtherProcessReadsExactSameData") == 0) {
        processForTestOneProcessWritesTheOtherProcessReadsExactSameData(streamHandle);
    } else if (strcmp(argv[1], "testOneProcessFillsStreamTheOtherProcessTryToWriteReturnsZero") == 0) {
        processForTestOneProcessFillsStreamTheOtherProcessTryToWriteReturnsZero(streamHandle);
    } else {
        return -1;
    }
    TEST_RESULTS();
    return 0;
}
