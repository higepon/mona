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

int main(int argc, char *argv[])
{
    testOneProcessReadsEmptyStreamImmediatlyReturnsZero();
    // testOneProcessWritesAndReadsExactSameData();
    // testTwoOneProcessWritesTheOtherProcessReadsExactSameData();
    // testOneProcessFillsStreamTheOtherProcessTryToWriteReturnsZero();
    // testOneProcessWaitsToReadTheOtherProcessWritesSomeAndNotifiesToTheWaitingProcess();
    // Ensures only one thread waiting.
    // remove uint8_t*
    TEST_RESULTS();
    return 0;
}
