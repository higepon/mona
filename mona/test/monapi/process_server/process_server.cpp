#include <monapi.h>
#define MUNIT_GLOBAL_VALUE_DEFINED
#include <monapi/MUnit.h>

using namespace MonAPI;

static uint32_t executeProcessWithStdHandle(uint32_t stdinHandle, uint32_t stdoutHandle)
{
    uint32_t tid;
    intptr_t ret = monapi_call_process_execute_file_get_tid(
        "/APPS/TWAIT.EX5",
        MONAPI_TRUE,
        &tid,
        stdinHandle,
        stdoutHandle);
    ASSERT_EQ(M_OK, ret);
    return tid;
}

static void terminateProcess(uint32_t tid)
{
    intptr_t ret = Message::send(tid, MSG_STOP);
    ASSERT_EQ(M_OK, ret);
}

static const int TEST_STDOUT_HANDLE  = 0x12345678;
static const int TEST_STDIN_HANDLE   = 0x87654321;
static const int TEST_STDOUT_HANDLE2 = 0x10000000;
static const int TEST_STDIN_HANDLE2  = 0x20000000;


static void test_returnsProcessStdHandle()
{
    uint32_t tid = executeProcessWithStdHandle(TEST_STDIN_HANDLE, TEST_STDOUT_HANDLE);
    EXPECT_EQ(TEST_STDOUT_HANDLE, System::getProcessStdoutID(tid));
    EXPECT_EQ(TEST_STDIN_HANDLE, System::getProcessStdinID(tid));
    terminateProcess(tid);
}

static void test_returnsInvalidStdHandleWhenThreadWasTerminated()
{
    uint32_t tid = executeProcessWithStdHandle(TEST_STDIN_HANDLE, TEST_STDOUT_HANDLE);
    terminateProcess(tid);
    sleep(100);
    EXPECT_EQ(THREAD_UNKNOWN, System::getProcessStdoutID(tid));
    EXPECT_EQ(THREAD_UNKNOWN, System::getProcessStdinID(tid));
}

static void test_distinguishesTwoProcessesWhichHaveSameName()
{
    uint32_t tid1 = executeProcessWithStdHandle(TEST_STDIN_HANDLE, TEST_STDOUT_HANDLE);
    uint32_t tid2 = executeProcessWithStdHandle(TEST_STDIN_HANDLE2, TEST_STDOUT_HANDLE2);
    logprintf("[tid1=%x tid2=%x]", tid1, tid2);

    EXPECT_EQ(TEST_STDOUT_HANDLE, System::getProcessStdoutID(tid1));
    EXPECT_EQ(TEST_STDIN_HANDLE, System::getProcessStdinID(tid1));
    EXPECT_EQ(TEST_STDOUT_HANDLE2, System::getProcessStdoutID(tid2));
    EXPECT_EQ(TEST_STDIN_HANDLE2, System::getProcessStdinID(tid2));

    terminateProcess(tid1);
    terminateProcess(tid2);
}

static void __fastcall subThread(void* arg)
{
    uint32_t mainThread = (uint32_t)arg;
    uint32_t stdinHandle = System::getProcessStdinID();
    Message::send(mainThread, MSG_STARTED, stdinHandle);
}

static void test_subThreadInheritsStdHandleFromMainThread()
{
    uintptr_t subThreadId = monapi_thread_create_with_arg(subThread, (void*)System::getThreadID());

    for (MessageInfo msg;;) {
        if (Message::receive(&msg) != 0) {
            continue;
        }
        if (msg.header == MSG_STARTED && msg.from == subThreadId) {
            uintptr_t mainStdinHandle = System::getProcessStdinID();
            uintptr_t subStdinHandle = msg.arg1;
            EXPECT_EQ(mainStdinHandle, subStdinHandle);
            break;
        }
    }
}

static void test_notifiesExitStatusToParent()
{
    uint32_t tid = executeProcessWithStdHandle(TEST_STDIN_HANDLE, TEST_STDOUT_HANDLE);
    terminateProcess(tid);
    EXPECT_EQ(ABNORMAL_STATUS, monapi_process_wait_terminated(tid));
}

int main(int argc, char *argv[])
{
    test_returnsProcessStdHandle();
    test_returnsInvalidStdHandleWhenThreadWasTerminated();

    test_distinguishesTwoProcessesWhichHaveSameName();
    test_subThreadInheritsStdHandleFromMainThread();
    test_notifiesExitStatusToParent();
    TEST_RESULTS(process_server);
    return 0;
}
