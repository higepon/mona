#include <monapi.h>
#include <map>
#define MUNIT_GLOBAL_VALUE_DEFINED
#include <monapi/MUnit.h>
#include <monapi/net.h>

using namespace MonAPI;

enum {
    MSG_SEND_BUFFER_START,
    MSG_SEND_BUFFER_PACKET,
    MSG_SEND_TEST
};


static intptr_t sendBuffer(uintptr_t dest, const void* buffer, uintptr_t bufferSize)
{
    MessageInfo msg;
    msg.header = MSG_SEND_BUFFER_START;
    msg.arg1 = bufferSize;
    uintptr_t sentSize = 0;
    bool isFirst = true;
    for (;;) {
        if (isFirst) {
            msg.header = MSG_SEND_BUFFER_START;
            isFirst = false;
        } else {
            msg.header = MSG_SEND_BUFFER_PACKET;
        }
        uintptr_t restSize = bufferSize - sentSize;
        uintptr_t sizeToSend = restSize > MESSAGE_INFO_MAX_STR_LENGTH ? MESSAGE_INFO_MAX_STR_LENGTH : restSize;
        memcpy(msg.str, (uint8_t*)buffer + sentSize, sizeToSend);
        sentSize += sizeToSend;
        if (Message::send(dest, &msg) != M_OK) {
            return M_BUSY;
        }
        if (sentSize == bufferSize) {
            return M_OK;
        }
    }
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
    for (;;) {
        MessageInfo msg;
        if (Message::receive(&msg) != M_OK) {
            continue;
        }

        if (msg.header == MSG_SEND_TEST) {
            TestInfo* testInfo = (TestInfo*)msg.arg1;
            memset(testInfo->buffer, 0xc1, testInfo->size);
//    intptr_t ret = Message::sendBuffer((uintptr_t)mainThread, buffer, BUFFER_SIZE);
            intptr_t ret =sendBuffer(testInfo->mainThread, testInfo->buffer, testInfo->size);
            EXPECT_EQ(M_OK, ret);
        }
    }
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

static bool isSendBufferPacket(MessageInfo* msg1, MessageInfo* msg2)
{
    if (msg1->header == MSG_SEND_BUFFER_PACKET ||
        msg1->header == MSG_SEND_BUFFER_START) {
        if (msg1->from == msg2->from) {
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

void testSendReceive(uintptr_t tid, TestInfo* testInfo)
{
    Receivers receivers;

    Message::send(tid, MSG_SEND_TEST, (uintptr_t)testInfo);

    for (;;) {
        MessageInfo expectedMsg;
        expectedMsg.from = tid;
        MessageInfo msg;
        if (Message::receive(&msg, &expectedMsg, isSendBufferPacket) != M_OK) {
            continue;
        }

        BufferReceiver* receiver;
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
            EXPECT_EQ(receiver->bufferSize(), testInfo->size);
            EXPECT_EQ(0, memcmp(receiver->buffer(), testInfo->buffer, testInfo->size));
            delete receiver;
            break;
        }
    }
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

int main(int argc, char *argv[])
{
//    DebuggerService::breakpoint();

    testSendBuffer();
    TEST_RESULTS(monapi_message);
    return 0;
}
