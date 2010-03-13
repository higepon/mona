/*----------------------------------------------------------------------
    Message
----------------------------------------------------------------------*/

#include <monapi/Message.h>
#include <monapi/BufferReceiver.h>
#include <monapi/syscall.h>
#include <monapi/string.h>
#include <monapi/Assert.h>
#include <sys/error.h>

namespace MonAPI {

int Message::send(uint32_t tid, MessageInfo* info)
{
    if (tid == THREAD_UNKNOWN) return 1;
    return syscall_send(tid, info);
}

int Message::send(uint32_t tid, uint32_t header, uint32_t arg1 /*= 0*/, uint32_t arg2 /*= 0*/, uint32_t arg3 /*= 0*/, const char* str /*= NULL */)
{
    MessageInfo info;
    Message::create(&info, header, arg1, arg2, arg3, str);
    return Message::send(tid, &info);
}

int Message::receive(MessageInfo* info)
{
    int result = syscall_receive(info);
    if (result != M_OK)
    {
         syscall_mthread_yield_message();
         result = syscall_receive(info);
    }
    return result;
}

int Message::sendReceiveA(MessageInfo* dst, uint32_t tid, MessageInfo* info)
{
    MessageInfo src;

    int result = Message::send(tid, info);

    if (result != M_OK) return result;

    src.from = tid;
    src.header = MSG_RESULT_OK;
    src.arg1 = info->header;
    return Message::receive(dst, &src, Message::equalsFromHeaderArg1);
}

int Message::sendReceive(MessageInfo* dst, uint32_t tid, uint32_t header, uint32_t arg1 /* = 0 */, uint32_t arg2 /* = 0 */, uint32_t arg3 /* = 0 */, const char* str /* = NULL */)
{
    MessageInfo src;

    int result = Message::send(tid, header, arg1, arg2, arg3, str);
    if (result != M_OK) return result;
    src.from = tid;
    src.header = MSG_RESULT_OK;
    src.arg1 = header;

    return Message::receive(dst, &src, Message::equalsFromHeaderArg1);
}

int Message::reply(MessageInfo* info, uint32_t arg2 /* = 0 */, uint32_t arg3 /* = 0 */, const char* str /* = NULL */)
{
    return Message::send(info->from, MSG_RESULT_OK, info->header, arg2, arg3, str);
}

int Message::replyError(MessageInfo* info, uint32_t arg2 /* = 0 */, uint32_t arg3 /* = 0 */, const char* str /* = NULL */)
{
    return Message::send(info->from, MSG_RESULT_ERROR, info->header, arg2, arg3, str);
}


int Message::receive(MessageInfo* dst, MessageInfo* src, bool(*equals)(MessageInfo* msg1, MessageInfo* msg2))
{
    MessageInfo msg;
    for (int i = 0; ; i++)
    {
        int result = Message::peek(&msg, i);

        if (result != M_OK)
        {
            i--;
            syscall_mthread_yield_message();
        }
        else if ((*equals)(&msg, src))
        {
            Message::peek(&msg, i, PEEK_REMOVE);
            break;
        }
    }

    if (dst != NULL) *dst = msg;
    return M_OK;
}

int Message::peek(MessageInfo* info, int index, int flags)
{
    return syscall_peek(info, index, flags);
}

void Message::create(MessageInfo* info, uint32_t header, uint32_t arg1 /*= 0*/, uint32_t arg2 /*= 0*/, uint32_t arg3 /*= 0*/, const char* str /*= NULL */)
{
    info->header = header;
    info->arg1 = arg1;
    info->arg2 = arg2;
    info->arg3 = arg3;

    if (str != NULL)
    {
//        strncpy(info->str, str, sizeof(info->str));
        memcpy(info->str, str, MESSAGE_INFO_MAX_STR_LENGTH);
    }
    return;
}

bool Message::exist()
{
    return syscall_exist_message();
}

uint32_t Message::lookup(const char* name)
{
    return syscall_lookup(name);
}

uint32_t Message::lookupMainThread(const char* name)
{
    return syscall_lookup_main_thread(name);
}

uint32_t Message::lookupMainThread()
{
    return syscall_lookup_main_thread(NULL);
}

intptr_t Message::sendBuffer(uintptr_t dest, const void* buffer, uintptr_t bufferSize)
{
    MessageInfo msg;
    msg.header = Message::MSG_SEND_BUFFER_START;
    msg.arg1 = bufferSize;
    uintptr_t sentSize = 0;
    bool isFirst = true;
    for (;;) {
        if (isFirst) {
            msg.header = Message::MSG_SEND_BUFFER_START;
            isFirst = false;
        } else {
            msg.header = Message::MSG_SEND_BUFFER_PACKET;
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

static bool isSendBufferPacket(MessageInfo* msg1, MessageInfo* msg2)
{
    if (msg1->header == Message::MSG_SEND_BUFFER_PACKET ||
        msg1->header == Message::MSG_SEND_BUFFER_START) {
        if (msg1->from == msg2->from) {
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

// caller should delete BufferReceiver.
BufferReceiver* Message::receiveBuffer(uintptr_t tid)
{
    for (;;) {
        MessageInfo expectedMsg;
        expectedMsg.from = tid;
        MessageInfo msg;
        if (Message::receive(&msg, &expectedMsg, isSendBufferPacket) != M_OK) {
            continue;
        }

        BufferReceiver* receiver;
        if (msg.header == Message::MSG_SEND_BUFFER_START) {
            uintptr_t bufferSize = msg.arg1;
            receiver = new BufferReceiver(bufferSize);
            receiver->receive(msg.str, MESSAGE_INFO_MAX_STR_LENGTH);
        } else if (msg.header == Message::MSG_SEND_BUFFER_PACKET) {
            ASSERT(receiver != NULL);
            receiver->receive(msg.str, MESSAGE_INFO_MAX_STR_LENGTH);
        }
        if (receiver->isDone()) {
            return receiver;
        }
    }
}

}
