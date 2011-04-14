#include <monapi.h>
#define MUNIT_GLOBAL_VALUE_DEFINED
#include <monapi/MUnit.h>

using namespace MonAPI;

static uint32_t executeProcessWithStdout(uint32_t stdoutHandle)
{
    uint32_t tid;
    intptr_t ret = monapi_call_process_execute_file_get_tid("/APPS/TWAIT.EX5",
                                             MONAPI_TRUE,
                                             &tid,
                                             System::getProcessStdinID(),
                                             stdoutHandle);
    ASSERT_EQ(M_OK, ret);
    return tid;
}

static void terminateProcess(uint32_t tid)
{
    intptr_t ret = Message::send(tid, MSG_STOP);
    ASSERT_EQ(M_OK, ret);
}

static void test_returnsProcessStdoutHandle()
{
    const int TEST_STDOUT_HANDLE= 0x12345678;
    uint32_t tid = executeProcessWithStdout(TEST_STDOUT_HANDLE);;
    EXPECT_EQ(TEST_STDOUT_HANDLE, System::getProcessStdoutID(tid));
    terminateProcess(tid);
}

static void test_returnsInvalidStdoutWhenThreadWasTerminated()
{
    const int TEST_STDOUT_HANDLE= 0x12345678;
    uint32_t tid = executeProcessWithStdout(TEST_STDOUT_HANDLE);;
    terminateProcess(tid);
    sleep(500);
    EXPECT_EQ(THREAD_UNKNOWN, System::getProcessStdoutID(tid));
}

int main(int argc, char *argv[])
{
    test_returnsProcessStdoutHandle();
    test_returnsInvalidStdoutWhenThreadWasTerminated();
    TEST_RESULTS(process_server);
    return 0;
}
