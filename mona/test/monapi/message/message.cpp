#include <monapi.h>
#define MUNIT_GLOBAL_VALUE_DEFINED
#include <monapi/MUnit.h>
#include <monapi/net.h>

using namespace MonAPI;

static const uintptr_t BUFFER_SIZE = 1000;
static uint8_t buffer[BUFFER_SIZE];

enum {
    MSG_SEND_BUFFER_START,
    MSG_SEND_BUFFER_PACKET
};

static intptr_t sendBuffer(uintptr_t dest, const uint8_t* buffer, uintptr_t bufferSize)
{
    if (bufferSize == 0) {
        return M_OK;
    }

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

static void __fastcall sendThread(void* mainThread)
{
    memset(buffer, BUFFER_SIZE, 0xc1);
//    intptr_t ret = Message::sendBuffer((uintptr_t)mainThread, buffer, BUFFER_SIZE);
    intptr_t ret =sendBuffer((uintptr_t)mainThread, buffer, BUFFER_SIZE);
    EXPECT_EQ(M_OK, ret);
    exit(0);
}

void testSendBuffer()
{
    uintptr_t mainThread = System::getThreadID();
    syscall_mthread_create_with_arg(sendThread, (void*)mainThread);
    uint8_t* received = NULL;
    uintptr_t receivedSize = 0;
    uintptr_t bufferSize = 0;
    for (;;) {
        MessageInfo msg;
        if (Message::receive(&msg) != M_OK) {
            continue;
        }
        if (msg.header == MSG_SEND_BUFFER_START) {
            bufferSize = msg.arg1;
            received = new uint8_t[bufferSize];
            ASSERT_TRUE(received != NULL);
            uintptr_t sizeToReceive = MESSAGE_INFO_MAX_STR_LENGTH > bufferSize ? bufferSize : MESSAGE_INFO_MAX_STR_LENGTH;
            memcpy(received + receivedSize, msg.str, sizeToReceive);
            receivedSize += sizeToReceive;
        } else if (msg.header == MSG_SEND_BUFFER_PACKET) {
            uintptr_t restSize = bufferSize - receivedSize;
            ASSERT_TRUE(restSize != 0);
            uintptr_t sizeToReceive = MESSAGE_INFO_MAX_STR_LENGTH > restSize ? restSize : MESSAGE_INFO_MAX_STR_LENGTH;
            memcpy(received + receivedSize, msg.str, sizeToReceive);
            receivedSize += sizeToReceive;
        }
        if (receivedSize == bufferSize) {
            break;
        }
    }
    EXPECT_EQ(receivedSize, BUFFER_SIZE);
    EXPECT_EQ(0, memcmp(received, buffer, BUFFER_SIZE));
    // todo check pid

}

int main(int argc, char *argv[])
{
    testSendBuffer();
    TEST_RESULTS(monapi_message);
    return 0;
}
