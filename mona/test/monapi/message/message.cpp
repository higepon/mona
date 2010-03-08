#include <monapi.h>
#include <map>
#define MUNIT_GLOBAL_VALUE_DEFINED
#include <monapi/MUnit.h>
#include <monapi/net.h>

using namespace MonAPI;

enum {
    MSG_SEND_BUFFER_START,
    MSG_SEND_BUFFER_PACKET
};

static intptr_t sendBuffer(uintptr_t dest, const uint8_t* buffer, uintptr_t bufferSize)
{
    MessageInfo msg;
    msg.header = MSG_SEND_BUFFER_START;
    msg.arg1 = bufferSize;
    uintptr_t sizeToSend = bufferSize > MESSAGE_INFO_MAX_STR_LENGTH ? MESSAGE_INFO_MAX_STR_LENGTH : bufferSize;
    memcpy(msg.str, buffer, sizeToSend);
    if (Message::send(dest, &msg) != M_OK) {
        return M_BUSY;
    }
    uintptr_t sentSize = sizeToSend;
    for (;;) {
        if (sentSize == bufferSize) {
            break;
        }
        msg.header = MSG_SEND_BUFFER_PACKET;
        uintptr_t restSize = bufferSize - sentSize;
        uintptr_t sizeToSend = restSize > MESSAGE_INFO_MAX_STR_LENGTH ? MESSAGE_INFO_MAX_STR_LENGTH : restSize;
        memcpy(msg.str, buffer + sentSize, sizeToSend);
        sentSize += sizeToSend;
        if (Message::send(dest, &msg) != M_OK) {
            return M_BUSY;
        }
    }
    return M_OK;
}

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
    TestInfo* testInfo = (TestInfo*)arg;
    memset(testInfo->buffer, 0xc1, testInfo->size);
//    intptr_t ret = Message::sendBuffer((uintptr_t)mainThread, buffer, BUFFER_SIZE);
    intptr_t ret =sendBuffer(testInfo->mainThread, testInfo->buffer, testInfo->size);
    EXPECT_EQ(M_OK, ret);
    exit(0);
}



struct BufferReceiveState
{
    BufferReceiveState(uintptr_t bufferSize) : buffer(new uint8_t[bufferSize]),
                                               bufferSize(bufferSize),
                                               receivedSize(0)
    {
    }
    ~BufferReceiveState()
    {
        delete[] buffer;
    }
    uint8_t* buffer;
    uintptr_t bufferSize;
    uintptr_t receivedSize;
};

typedef std::map<uintptr_t, BufferReceiveState*> Buffers;

void testSendReceive(uintptr_t size)
{
    uintptr_t mainThread = System::getThreadID();
    TestInfo testInfo(mainThread, size);
    syscall_mthread_create_with_arg(sendThread, (void*)&testInfo);
    BufferReceiveState* state;
//     uint8_t* received = NULL;
//     uintptr_t receivedSize = 0;
//     uintptr_t bufferSize = 0;

    Buffers buffers;

    for (;;) {
        MessageInfo msg;
        if (Message::receive(&msg) != M_OK) {
            continue;
        }
        if (msg.header == MSG_SEND_BUFFER_START) {
            uintptr_t bufferSize = msg.arg1;
            state = new BufferReceiveState(bufferSize);
            buffers[msg.from] = state;
            uintptr_t sizeToReceive = MESSAGE_INFO_MAX_STR_LENGTH > bufferSize ? bufferSize : MESSAGE_INFO_MAX_STR_LENGTH;
            memcpy(state->buffer, msg.str, sizeToReceive);
            state->receivedSize += sizeToReceive;
            if (state->receivedSize == state->bufferSize) {
                EXPECT_EQ(state->receivedSize, testInfo.size);
                EXPECT_EQ(0, memcmp(state->buffer, testInfo.buffer, testInfo.size));
                delete state;
                break;
            }

        } else if (msg.header == MSG_SEND_BUFFER_PACKET) {
            BufferReceiveState* s = buffers[msg.from];
            ASSERT_TRUE(s != NULL);
            uintptr_t restSize = s->bufferSize - s->receivedSize;
            ASSERT_TRUE(restSize != 0);
            uintptr_t sizeToReceive = MESSAGE_INFO_MAX_STR_LENGTH > restSize ? restSize : MESSAGE_INFO_MAX_STR_LENGTH;
            memcpy(s->buffer + s->receivedSize, msg.str, sizeToReceive);
            s->receivedSize += sizeToReceive;
            if (s->receivedSize == s->bufferSize) {
                EXPECT_EQ(s->receivedSize, testInfo.size);
                EXPECT_EQ(0, memcmp(s->buffer, testInfo.buffer, testInfo.size));
                delete s;
                break;
            }

        }
    }
}


void testSendBuffer()
{
    const uintptr_t MAX_TEST_BUFFER_SIZE = MESSAGE_INFO_MAX_STR_LENGTH * 2 + 1;
    for (uintptr_t testBufferSize = 1; testBufferSize < MAX_TEST_BUFFER_SIZE; testBufferSize++) {
        testSendReceive(testBufferSize);
    }
    // todo check pid
    // todo zero size
}

int main(int argc, char *argv[])
{
//    DebuggerService::breakpoint();

    testSendBuffer();
    TEST_RESULTS(monapi_message);
    return 0;
}
