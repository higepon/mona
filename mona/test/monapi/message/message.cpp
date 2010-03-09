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



class BufferReceiver
{
public:
    BufferReceiver(uintptr_t bufferSize) : buffer_(new uint8_t[bufferSize]),
                                           bufferSize_(bufferSize),
                                           receivedSize_(0)
    {
    }
    ~BufferReceiver()
    {
        delete[] buffer_;
    }

    uintptr_t bufferSize() const
    {
        return bufferSize_;
    }

    bool isDone() const
    {
        return bufferSize_ == receivedSize_;
    }

    uint8_t* buffer() const
    {
        return buffer_;
    }

    uintptr_t restSizeToReceive() const
    {
        return bufferSize_ - receivedSize_;
    }

    bool receive(const void* source, uintptr_t maxSourceSize)
    {
        ASSERT_TRUE(!isDone());
        uintptr_t sizeToReceive = maxSourceSize > restSizeToReceive() ? restSizeToReceive() : maxSourceSize;
        memcpy(buffer_ + receivedSize_, source, sizeToReceive);
        receivedSize_ += sizeToReceive;

    }

private:
    uint8_t* buffer_;
    uintptr_t bufferSize_;
    uintptr_t receivedSize_;
};

typedef std::map<uintptr_t, BufferReceiver*> Receivers;

void testSendReceive(uintptr_t size)
{
    uintptr_t mainThread = System::getThreadID();
    TestInfo testInfo(mainThread, size);
    syscall_mthread_create_with_arg(sendThread, (void*)&testInfo);
    BufferReceiver* receiver;

    Receivers receivers;

    for (;;) {
        MessageInfo msg;
        if (Message::receive(&msg) != M_OK) {
            continue;
        }
        if (msg.header == MSG_SEND_BUFFER_START) {
            uintptr_t bufferSize = msg.arg1;
            receiver = new BufferReceiver(bufferSize);
            receivers[msg.from] = receiver;
            receiver->receive(msg.str, MESSAGE_INFO_MAX_STR_LENGTH);
        } else if (msg.header == MSG_SEND_BUFFER_PACKET) {
            receiver = receivers[msg.from];
            ASSERT_TRUE(receiver != NULL);
            receiver->receive(msg.str, MESSAGE_INFO_MAX_STR_LENGTH);
        }
        if (receiver->isDone()) {
            EXPECT_EQ(receiver->bufferSize(), testInfo.size);
            EXPECT_EQ(0, memcmp(receiver->buffer(), testInfo.buffer, testInfo.size));
            delete receiver;
            break;
        }
    }
}


void testSendBuffer()
{
    const uintptr_t MAX_TEST_BUFFER_SIZE = MAX_TEST_BUFFER_SIZE * 2 + 1;
    testSendReceive(1);
//     for (uintptr_t testBufferSize = 1; testBufferSize < MAX_TEST_BUFFER_SIZE; testBufferSize++) {
//         testSendReceive(testBufferSize);
//     }
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
