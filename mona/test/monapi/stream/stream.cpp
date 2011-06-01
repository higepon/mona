#include <monapi.h>

#define MUNIT_GLOBAL_VALUE_DEFINED
#include <monapi/MUnit.h>

using namespace MonAPI;

int main(int argc, char *argv[])
{
    // testOneProcessReadsEmptyStreamImmediatlyReturnsZero();
    // testOneProcessWritesAndReadsExactSameData();
    // testTwoOneProcessWritesTheOtherProcessReadsExactSameData();
    // testOneProcessFillsStreamTheOtherProcessTryToWriteReturnsZero();
    // testOneProcessWaitsToReadTheOtherProcessWritesSomeAndNotifiesToTheWaitingProcess();
    // Ensures only one thread waiting.
    TEST_RESULTS();
    return 0;
}
