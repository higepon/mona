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

static const uint8_t TEST_DATA[] = {0x01, 0x02, 0x03};
static const int DATA_SIZE = sizeof(TEST_DATA);
static uintptr_t mainThread = THREAD_UNKNOWN;

static void testOneProcessWritesAndReadsExactSameData()
{
    Stream s;
    EXPECT_EQ(DATA_SIZE, s.write(TEST_DATA, DATA_SIZE));
    uint8_t buf[DATA_SIZE];
    EXPECT_EQ(DATA_SIZE, s.read(buf, DATA_SIZE));
    EXPECT_EQ(0, memcmp(TEST_DATA, buf, DATA_SIZE));
    EXPECT_EQ(0, s.read(buf, DATA_SIZE));
}

static void notifyDone(uintptr_t dest)
{
    ASSERT_EQ(M_OK, Message::send(dest, MSG_OK));
}

static void waitSubThreadDone(uintptr_t tid)
{
    MessageInfo dst, src;
    src.header = MSG_OK;
    int ret = Message::receive(&dst, &src, Message::equalsHeader);
    ASSERT_EQ(M_OK, ret);
    ASSERT_TRUE(dst.from == tid);
}

static void __fastcall readSubThread(void* arg)
{
    scoped_ptr<Stream> s(Stream::FromHandle((uintptr_t)arg));
    uint8_t buf[DATA_SIZE];
    EXPECT_EQ(DATA_SIZE, s->read(buf, DATA_SIZE));
    EXPECT_EQ(0, memcmp(TEST_DATA, buf, DATA_SIZE));
    notifyDone(mainThread);
}

static void testOneProcessWritesTheOtherProcessReadsExactSameData()
{
    Stream s;
    ASSERT_EQ(DATA_SIZE, s.write(TEST_DATA, DATA_SIZE));
    mainThread = System::getThreadID();
    uintptr_t tid = monapi_thread_create_with_arg(readSubThread, (void*)s.handle());
    waitSubThreadDone(tid);
}

int main(int argc, char *argv[])
{
    testOneProcessReadsEmptyStreamImmediatlyReturnsZero();
    testOneProcessWritesAndReadsExactSameData();
    testOneProcessWritesTheOtherProcessReadsExactSameData();
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
