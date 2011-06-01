#include <monapi.h>

#define MUNIT_GLOBAL_VALUE_DEFINED
#include <monapi/MUnit.h>
#include <limits.h>
#include <stdio.h>
#include <string>

using namespace MonAPI;

static void testOneProcessReadsEmptyStreamImmediatlyReturnsZero()
{
    Stream s;
    uint8_t buf[3];
    EXPECT_EQ(0, s.read(buf, sizeof(buf)));
}

#include "../stream/test_data.c"

static void testOneProcessWritesAndReadsExactSameData()
{
    Stream s;
    EXPECT_EQ(DATA_SIZE, s.write(TEST_DATA, DATA_SIZE));
    uint8_t buf[DATA_SIZE];
    EXPECT_EQ(DATA_SIZE, s.read(buf, DATA_SIZE));
    EXPECT_EQ(0, memcmp(TEST_DATA, buf, DATA_SIZE));
    EXPECT_EQ(0, s.read(buf, DATA_SIZE));
}

static int executeSubProcess(const std::string& funcName, uint32_t streamHandle)
{
    std::string command("/APPS/TSUBSTRM.EX5");
    command += " ";
    command += funcName;
    char buf[32];
    sprintf(buf, " %x", streamHandle);
    command += buf;
    uint32_t tid;
    intptr_t result = monapi_process_execute_file_get_tid(
        command.c_str(),
        MONAPI_TRUE,
        &tid,
        MonAPI::System::getProcessStdinID(),
        MonAPI::System::getProcessStdoutID());
    if (result != 0) {
        EXPECT_TRUE(false);
        return -1;
    }
    return monapi_process_wait_terminated(tid);
}


static void testOneProcessWritesTheOtherProcessReadsExactSameData()
{
    Stream s;
    ASSERT_EQ(DATA_SIZE, s.write(TEST_DATA, DATA_SIZE));
    EXPECT_EQ(0, executeSubProcess(__func__, s.handle()));
}

static void testOneProcessFillsStreamTheOtherProcessTryToWriteReturnsZero()
{
    const int STREAM_SIZE_BYTE = 3;
    Stream s(STREAM_SIZE_BYTE);
    ASSERT_EQ(STREAM_SIZE_BYTE, s.write(TEST_DATA, STREAM_SIZE_BYTE));
    EXPECT_EQ(0, executeSubProcess(__func__, s.handle()));
}

// Some of following tests depend on test/stream-sub/.
int main(int argc, char *argv[])
{
    testOneProcessReadsEmptyStreamImmediatlyReturnsZero();
    testOneProcessWritesAndReadsExactSameData();
    testOneProcessWritesTheOtherProcessReadsExactSameData();
    testOneProcessFillsStreamTheOtherProcessTryToWriteReturnsZero();
    // testOneProcessWaitsToReadTheOtherProcessWritesSomeAndNotifiesToTheWaitingProcess();
    // fromHandle
    // tryLock is used?
    // isInvalid work?
    // we never require three mutex.
    // Ensures only one thread waiting.
    // remove uint8_t*
    // make clean doesn't work for some tests
    // Stream.cpp:288
    // semaphore on syscalls.cpp is necessar?
    TEST_RESULTS();
    return 0;
}
