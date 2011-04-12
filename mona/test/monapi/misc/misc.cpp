#include <monapi.h>
#define MUNIT_GLOBAL_VALUE_DEFINED
#include <monapi/MUnit.h>
#include <monapi/net.h>
#include <monapi/Buffer.h>
#include <monapi.h>
#include <stdio.h>
#include <limits.h>
#include <string>

using namespace MonAPI;

enum {
    MSG_SEND_TEST
};

void testDate()
{
    uint64_t msec1 = Date::nowInMsec();
    sleep(100);
    uint64_t msec2 = Date::nowInMsec();
    uint64_t diff = msec2 - msec1;
    EXPECT_TRUE(diff > 70);
    EXPECT_TRUE(diff < 130);
}

void testThreadSelf()
{
    uintptr_t self = syscall_mthread_self();
    EXPECT_EQ(self, System::getThreadID());
}

static void __fastcall infiniteThread(void* arg)
{
    for (;;) {
        MessageInfo msg;
        if (Message::receive(&msg) != M_OK) {
            continue;
        }
    }
    exit(0);
}


void testThreadKill()
{
    uintptr_t tid = syscall_mthread_create_with_arg(infiniteThread, NULL);
    EXPECT_EQ(M_OK, Message::send(tid, MSG_SEND_TEST));
    EXPECT_EQ(M_OK, syscall_mthread_kill(tid));
    SKIP(EXPECT_EQ(M_BAD_THREAD_ID, Message::send(tid, MSG_SEND_TEST)));
}

void testNet()
{
    EXPECT_EQ(0x3412, htons(0x1234));
}

static void testISO9600_file_read()
{
    intptr_t handle = monapi_file_open("/LIBS/SCHEME/fib.scm", 0);
    scoped_ptr<SharedMemory> shm(monapi_file_read(handle, 4096));
    ASSERT_TRUE(shm.get() != NULL);
    EXPECT_TRUE(shm->size() > 0);
    // reached EOF
    scoped_ptr<SharedMemory> shm2(monapi_file_read(handle, 4096));
    EXPECT_TRUE(NULL == shm2.get());
    monapi_file_close(handle);
}

static void test_Page_fault_handler_should_be_fast()
{
    const int SIZE = 3 * 1024 * 1024;
    scoped_ptr<SharedMemory> mi(new SharedMemory(SIZE));
    ASSERT_EQ(M_OK, mi->map(true));
    uint64_t s1 = MonAPI::Date::nowInMsec();
    memset(mi->data(), 0, SIZE);
    uint64_t e1 = MonAPI::Date::nowInMsec();
    EXPECT_TRUE((e1 - s1) <= 10);

    uint64_t s2 = MonAPI::Date::nowInMsec();
    memset(mi->data(), 0, SIZE);
    uint64_t e2 = MonAPI::Date::nowInMsec();
    EXPECT_EQ(0, e2 - s2);
}

static void testBuffer()
{
    const int BUFFER_SIZE = 32;;
    uint8_t data1[BUFFER_SIZE];
    uint8_t data2[BUFFER_SIZE];
    data1[0] = 0xff;
    data1[BUFFER_SIZE - 1] = 0xee;
    Buffer buf1(data1, BUFFER_SIZE);
    EXPECT_EQ(BUFFER_SIZE, buf1.size());
    buf1.fillZero();
    EXPECT_EQ(0, data1[0]);
    EXPECT_EQ(0, data1[BUFFER_SIZE - 1]);

    Buffer buf2(data2, BUFFER_SIZE);
    for (int i = 0; i < BUFFER_SIZE; i++) {
        data2[i] = i & 0xff;
    }

    EXPECT_TRUE(Buffer::copy(buf1, 0, buf2, 1, 3));
    EXPECT_EQ(1, data1[0]);
    EXPECT_EQ(2, data1[1]);
    EXPECT_EQ(3, data1[2]);
}

static void testBufferOverCopyShouldFail()
{
    const int BUFFER_SIZE = 32;;
    uint8_t data1[BUFFER_SIZE];
    uint8_t data2[BUFFER_SIZE];
    Buffer buf1(data1, BUFFER_SIZE);
    buf1.fillZero();
    Buffer buf2(data2, BUFFER_SIZE);
    for (int i = 0; i < BUFFER_SIZE; i++) {
        data2[i] = i & 0xff;
    }

    EXPECT_EQ(true, Buffer::copy(buf1, 0, buf2, 0, BUFFER_SIZE));
    EXPECT_EQ(false, Buffer::copy(buf1, 0, buf2, 0, BUFFER_SIZE + 1));
    EXPECT_EQ(true, Buffer::copy(buf1, 5, buf2, 0, BUFFER_SIZE - 5));
    EXPECT_EQ(false, Buffer::copy(buf1, 5, buf2, 0, BUFFER_SIZE - 4));
    EXPECT_EQ(true, Buffer::copy(buf1, 0, buf2, 5, BUFFER_SIZE - 5));
    EXPECT_EQ(false, Buffer::copy(buf1, 0, buf2, 5, BUFFER_SIZE - 4));
    EXPECT_EQ(true, Buffer::copy(buf1, 5, buf2, 5, BUFFER_SIZE - 5));
    EXPECT_EQ(false, Buffer::copy(buf1, 5, buf2, 5, BUFFER_SIZE - 4));
}


static void testSharedMemory()
{
    const int SIZE = 3;
    SharedMemory shm(SIZE);
    EXPECT_EQ(false, shm.isMapped());
    EXPECT_EQ(0, shm.handle());
    EXPECT_EQ(NULL, shm.data());

    EXPECT_EQ(M_OK, shm.map());
    shm.data()[0] = 0xfe;
    EXPECT_EQ(0xfe, shm.data()[0]);
    EXPECT_EQ(true, shm.isMapped());
    EXPECT_EQ(M_BUSY, shm.map());
    EXPECT_TRUE(shm.data() != NULL);
    EXPECT_TRUE(shm.handle() > 0);
    EXPECT_EQ(SIZE, shm.size());
    EXPECT_EQ(M_OK, shm.unmap());
    EXPECT_EQ(0, shm.handle());
    EXPECT_EQ(NULL, shm.data());
    EXPECT_EQ(false, shm.isMapped());
}

static void testCreateProcessManyTimes()
{
    const int NTIMES = 200;
    std::string command(MonAPI::System::getMoshPath());
    command += " /USER/empty.sps";
    logprintf("calling mosh test ... ");
    for (int i = 0; i < NTIMES; i++) {
        logprintf("[%d]", i);
        uint32_t tid;
        intptr_t result = monapi_call_process_execute_file_get_tid("/APPS/TEMPTY.EX5", // command.c_str()
            //command.c_str(),
                                                                   MONAPI_TRUE,
                                                                   &tid,
                                                                   MonAPI::System::getProcessStdinID(),
                                                                   MonAPI::System::getProcessStdoutID());
        if (result != 0) {
            monapi_fatal("can't exec Mosh");
        }
        monapi_process_wait_terminated(tid);
    }
    logprintf("\n");
}
int main(int argc, char *argv[])
{
    testDate();
    testThreadSelf();
    testNet();
    testThreadKill();
    testISO9600_file_read();
    test_Page_fault_handler_should_be_fast();
    testBuffer();
    testBufferOverCopyShouldFail();
    testSharedMemory();
    testCreateProcessManyTimes();
    TEST_RESULTS(monapi_misc);
    return 0;
}
