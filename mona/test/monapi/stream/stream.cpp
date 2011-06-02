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

static uint32_t startSubProcess(const std::string& funcName, uint32_t streamHandle)
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
        return THREAD_UNKNOWN;
    }
    return tid;
}

static int callSubProcess(const std::string& funcName, uint32_t streamHandle)
{
    uint32_t tid = startSubProcess(funcName, streamHandle);
    if (tid == THREAD_UNKNOWN) {
        return -1;
    }
    return monapi_process_wait_terminated(tid);
}

static void testOneProcessWritesTheOtherProcessReadsExactSameData()
{
    Stream s;
    ASSERT_EQ(DATA_SIZE, s.write(TEST_DATA, DATA_SIZE));
    EXPECT_EQ(0, callSubProcess(__func__, s.handle()));
}

static void testOneProcessFillsStreamTheOtherProcessTryToWriteReturnsZero()
{
    const int STREAM_SIZE_BYTE = 3;
    Stream s(STREAM_SIZE_BYTE);
    ASSERT_EQ(STREAM_SIZE_BYTE, s.write(TEST_DATA, STREAM_SIZE_BYTE));
    EXPECT_EQ(0, callSubProcess(__func__, s.handle()));
}

static void testOneProcessWaitsToReadTheOtherProcessWritesSomeAndNotifiesToTheWaitingProcess()
{
    Stream s;
    uint8_t buf[DATA_SIZE];
    ASSERT_EQ(0, s.read(buf, DATA_SIZE));
    startSubProcess(__func__, s.handle());
    s.waitForRead();
    EXPECT_EQ(DATA_SIZE, s.read(buf, DATA_SIZE));
    EXPECT_EQ(0, memcmp(TEST_DATA, buf, DATA_SIZE));
}

static void testOneProcessWaitsToReadWithWaitsFullFlagTheOtherProcessWritesSomeAndNotifiesToTheWaitingProcess()
{
    Stream s;
    uint8_t buf[DATA_SIZE];
    ASSERT_EQ(0, s.read(buf, DATA_SIZE));
    startSubProcess("testOneProcessWaitsToReadTheOtherProcessWritesSomeAndNotifiesToTheWaitingProcess", s.handle());
    EXPECT_EQ(DATA_SIZE, s.read(buf, DATA_SIZE, true));
    EXPECT_EQ(0, memcmp(TEST_DATA, buf, DATA_SIZE));
}

static void testOneProcessWritesToFullStreamImmediatlyReturnsZero()
{
    const int STREAM_SIZE_BYTE = 3;
    Stream s(STREAM_SIZE_BYTE);
    ASSERT_EQ(STREAM_SIZE_BYTE, s.write(TEST_DATA, STREAM_SIZE_BYTE));
    EXPECT_EQ(0, callSubProcess(__func__, s.handle()));
}

static void testOneProcessWaitsToWriteTheOtherProcessReadsSomeAndNotifiesToTheWaitingProcess()
{
    Stream s(SMALL_STREAM_SIZE_BYTE);
    ASSERT_EQ(SMALL_STREAM_SIZE_BYTE, s.write(TEST_DATA, SMALL_STREAM_SIZE_BYTE));
    ASSERT_EQ(0, s.write(TEST_DATA, SMALL_STREAM_SIZE_BYTE));
    startSubProcess(__func__, s.handle());
    s.waitForWrite();
    ASSERT_EQ(SMALL_STREAM_SIZE_BYTE, s.write(TEST_DATA, SMALL_STREAM_SIZE_BYTE));
}

static void testOneProcessWaitsToWriteWithWaitEmptyFlagTheOtherProcessReadsSomeAndNotifiesToTheWaitingProcess()
{
    Stream s(SMALL_STREAM_SIZE_BYTE);
    ASSERT_EQ(SMALL_STREAM_SIZE_BYTE, s.write(TEST_DATA, SMALL_STREAM_SIZE_BYTE));
    // Same sub process that testOneProcessWaitsToWriteTheOtherProcessReadsSomeAndNotifiesToTheWaitingProcess uses.
    startSubProcess("testOneProcessWaitsToWriteTheOtherProcessReadsSomeAndNotifiesToTheWaitingProcess", s.handle());
    ASSERT_EQ(SMALL_STREAM_SIZE_BYTE, s.write(TEST_DATA, SMALL_STREAM_SIZE_BYTE, true));
}

static void testIsInvalidReturnsTrueForBadStreamHandle()
{
    scoped_ptr<Stream> s(Stream::createFromHandle(0x1234));
    EXPECT_TRUE(s->isInvalid());
}

// Some of following tests depend on test/stream-sub/.
int main(int argc, char *argv[])
{
    testOneProcessReadsEmptyStreamImmediatlyReturnsZero();
    testOneProcessWritesAndReadsExactSameData();
    testOneProcessWritesTheOtherProcessReadsExactSameData();
    testOneProcessFillsStreamTheOtherProcessTryToWriteReturnsZero();
    testOneProcessWaitsToReadTheOtherProcessWritesSomeAndNotifiesToTheWaitingProcess();
    testOneProcessWaitsToReadWithWaitsFullFlagTheOtherProcessWritesSomeAndNotifiesToTheWaitingProcess();
    testOneProcessWritesToFullStreamImmediatlyReturnsZero();
    testOneProcessWaitsToWriteTheOtherProcessReadsSomeAndNotifiesToTheWaitingProcess();
    testOneProcessWaitsToWriteWithWaitEmptyFlagTheOtherProcessReadsSomeAndNotifiesToTheWaitingProcess();
    testIsInvalidReturnsTrueForBadStreamHandle();
    TEST_RESULTS();
    return 0;
}
