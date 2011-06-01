#include <monapi.h>

#define MUNIT_GLOBAL_VALUE_DEFINED
#include <monapi/MUnit.h>

using namespace MonAPI;

static void testOneProcessReadsEmptyStreamImmediatlyReturnsZero()
{
    Stream s;
    uint8_t buf[3];
    EXPECT_EQ(0, s.read(buf, sizeof(buf)));
}

static void testOneProcessWritesAndReadsExactSameData()
{
    const uint8_t TEST_DATA[] = {0x01, 0x02, 0x03};
    const int DATA_SIZE = sizeof(TEST_DATA);
    Stream s;
    EXPECT_EQ(DATA_SIZE, s.write(TEST_DATA, DATA_SIZE));
    uint8_t buf[DATA_SIZE];
    EXPECT_EQ(DATA_SIZE, s.read(buf, DATA_SIZE));
    EXPECT_EQ(0, memcmp(TEST_DATA, buf, DATA_SIZE));
    EXPECT_EQ(0, s.read(buf, DATA_SIZE));
}

int main(int argc, char *argv[])
{
    testOneProcessReadsEmptyStreamImmediatlyReturnsZero();
    testOneProcessWritesAndReadsExactSameData();
    // testTwoOneProcessWritesTheOtherProcessReadsExactSameData();
    // testOneProcessFillsStreamTheOtherProcessTryToWriteReturnsZero();
    // testOneProcessWaitsToReadTheOtherProcessWritesSomeAndNotifiesToTheWaitingProcess();
    // fromHandle
    // tryLock is used?
    // isInvalid work?
    // we never require three mutex.
    // Ensures only one thread waiting.
    // remove uint8_t*
    TEST_RESULTS();
    return 0;
}
