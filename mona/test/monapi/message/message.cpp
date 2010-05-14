#include <monapi.h>
#include <map>
#define MUNIT_GLOBAL_VALUE_DEFINED
#include <monapi/MUnit.h>
#include <monapi/net.h>

using namespace MonAPI;

enum {
    MSG_SEND_TEST
};

struct TestInfo
{
    TestInfo(uintptr_t mainThread, uintptr_t size) : mainThread(mainThread), size(size)
    {
        buffer = new uint8_t[size];
    }
    ~TestInfo()
    {
        delete[] buffer;
    }
    uintptr_t mainThread;
    uintptr_t size;
    uint8_t* buffer;
};

static void __fastcall sendThread(void* arg)
{
    for (;;) {
        MessageInfo msg;
        if (Message::receive(&msg) != M_OK) {
            continue;
        }

        if (msg.header == MSG_SEND_TEST) {
            TestInfo* testInfo = (TestInfo*)msg.arg1;
            memset(testInfo->buffer, 0xc1, testInfo->size);
            intptr_t ret = Message::sendBuffer(testInfo->mainThread, testInfo->buffer, testInfo->size);
            EXPECT_EQ(M_OK, ret);
        }
    }
    exit(0);
}

static void __fastcall receiverThread(void* arg)
{
    for (;;) {
        syscall_mthread_yield_message();
    }
    exit(0);
}

void testSendReceive(uintptr_t tid, TestInfo* testInfo)
{
    Message::send(tid, MSG_SEND_TEST, (uintptr_t)testInfo);
    BufferReceiver* receiver = Message::receiveBuffer(tid);
    EXPECT_EQ(receiver->bufferSize(), testInfo->size);
    EXPECT_EQ(0, memcmp(receiver->buffer(), testInfo->buffer, testInfo->size));
    delete receiver;
}

void testSendBuffer()
{
    const uintptr_t MAX_TEST_BUFFER_SIZE = MESSAGE_INFO_MAX_STR_LENGTH * 2 + 1;
    uintptr_t mainThread = System::getThreadID();
    uintptr_t tid = syscall_mthread_create_with_arg(sendThread, NULL);

    for (uintptr_t testBufferSize = 0; testBufferSize < MAX_TEST_BUFFER_SIZE; testBufferSize++) {
        TestInfo testInfo(mainThread, testBufferSize);
        testSendReceive(tid, &testInfo);
    }
}

void testMessageOverflow()
{
    uintptr_t tid = syscall_mthread_create_with_arg(receiverThread, NULL);
    int i;
    for (i = 0; i < MAX_MESSAGES + 1; i++) {
        if (i == MAX_MESSAGES) {
            EXPECT_EQ(M_MESSAGE_OVERFLOW, Message::send(tid, MSG_SEND_TEST));
        } else {
            EXPECT_EQ(M_OK, Message::send(tid, MSG_SEND_TEST));
        }
    }
}

void testPeek()
{
    MessageInfo msg;
    // Expect we have no message.
    EXPECT_EQ(M_BAD_INDEX, Message::peek(&msg, 0));
}

int main(int argc, char *argv[])
{
    testPeek();
    testMessageOverflow();
    testSendBuffer();
    TEST_RESULTS(monapi_message);
    return 0;
}
