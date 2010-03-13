#ifndef _MONAPI_MESSAGE_
#define _MONAPI_MESSAGE_

#include <sys/types.h>

namespace MonAPI {

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

        bool receive(const void* source, uintptr_t maxSourceSize);

    private:
        uint8_t* buffer_;
        uintptr_t bufferSize_;
        uintptr_t receivedSize_;
    };


/*----------------------------------------------------------------------
    Message
----------------------------------------------------------------------*/
class Message {
public:
    enum
    {
        MSG_SEND_BUFFER_START = 0xd0b29ce, /* BUF:STT */
        MSG_SEND_BUFFER_PACKET = 0xd0af54e /* BUF:PKT */
    };
    static int send(uint32_t tid, MessageInfo* info);
    static int send(uint32_t tid, uint32_t header, uint32_t arg1 = 0, uint32_t arg2 = 0, uint32_t arg3 = 0, const char* str = NULL);
    static intptr_t sendBuffer(uintptr_t dest, const void* buffer, uintptr_t bufferSize);
    static int receive(MessageInfo* info);
    static BufferReceiver* receiveBuffer(uintptr_t tid);
    static int reply(MessageInfo* info, uint32_t arg2 = 0, uint32_t arg3 = 0, const char* str = NULL);
    static int replyError(MessageInfo* info, uint32_t arg2 = 0, uint32_t arg3 = 0, const char* str = NULL);
    static int peek(MessageInfo* info, int index, int flags = 0);
    static void create(MessageInfo* info, uint32_t header, uint32_t arg1 = 0, uint32_t arg2 = 0, uint32_t arg3 = 0, const char* str = NULL);
    static bool exist();
    static uint32_t lookup(const char* name);
    static uint32_t lookupMainThread(const char* name);
    static uint32_t lookupMainThread();

    static int sendReceiveA(MessageInfo* dst, uint32_t tid, MessageInfo* info);

    static int sendReceive(MessageInfo* dst, uint32_t tid, uint32_t header, uint32_t arg1 = 0, uint32_t arg2 = 0, uint32_t arg3 = 0, const char* str = NULL);

    static int receive(MessageInfo* dst, MessageInfo* src, bool(*equals)(MessageInfo* msg1, MessageInfo* msg2));

    static bool equalsHeader(MessageInfo* msg1, MessageInfo* msg2)
    {
        return (msg1->header == msg2->header);
    }

    static bool equalsFrom(MessageInfo* msg1, MessageInfo* msg2)
    {
        return (msg1->from == msg2->from);
    }

    static bool equalsFromHeader(MessageInfo* msg1, MessageInfo* msg2)
    {
        return (msg1->from == msg2->from) && (msg1->header == msg2->header);
    }

    static bool equalsFromHeaderArg1(MessageInfo* msg1, MessageInfo* msg2)
    {
        return (msg1->from == msg2->from) && (msg1->header == msg2->header) && (msg1->arg1 == msg2->arg1);
    }

};

}

#endif
