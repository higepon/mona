/*----------------------------------------------------------------------
    Message
----------------------------------------------------------------------*/
#ifdef ON_LINUX
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "SocketMessage.h"
#include "types.h"
#include "ProcessUtil.h"
#else
#include <monapi/Message.h>
#include <monapi/syscall.h>
#include <monapi/string.h>
#endif

namespace MonAPI {

#ifdef ON_LINUX
using namespace mona_helper;

void Message::initialize()
{
    SocketMessage::initialize();
}

void Message::destroy()
{
}
#endif

int Message::send(dword tid, MessageInfo* info)
{
#ifdef ON_LINUX
    return SocketMessage::send(tid, info);
#else
    if (tid == THREAD_UNKNOWN) return 1;
    return syscall_send(tid, info);
#endif
}

int Message::receive(MessageInfo* info)
{
#ifdef ON_LINUX
    return SocketMessage::receive(info);
#endif
}

int Message::send(dword tid, dword header, dword arg1 /*= 0*/, dword arg2 /*= 0*/, dword arg3 /*= 0*/, const char* str /*= NULL */)
{
    MessageInfo info;
    Message::create(&info, header, arg1, arg2, arg3, str);
    return Message::send(tid, &info);
}


int Message::sendReceiveA(MessageInfo* dst, dword tid, MessageInfo* info)
{
    MessageInfo src;

    int result = Message::send(tid, info);

    if (result != 0) return result;

    src.from = tid;
    src.header = MSG_RESULT_OK;
    src.arg1 = info->header;
    return Message::receive(dst, &src, Message::equalsFromHeaderArg1);
}

int Message::sendReceive(MessageInfo* dst, dword tid, dword header, dword arg1 /* = 0 */, dword arg2 /* = 0 */, dword arg3 /* = 0 */, const char* str /* = NULL */)
{
    MessageInfo src;

    int result = Message::send(tid, header, arg1, arg2, arg3, str);

    if (result != 0) return result;

    src.from = tid;
    src.header = MSG_RESULT_OK;
    src.arg1 = header;

    return Message::receive(dst, &src, Message::equalsFromHeaderArg1);
}

int Message::reply(MessageInfo* info, dword arg2 /* = 0 */, dword arg3 /* = 0 */, const char* str /* = NULL */)
{
    sleep(1);
    return Message::send(info->from, MSG_RESULT_OK, info->header, arg2, arg3, str);
}

int Message::receive(MessageInfo* dst, MessageInfo* src, bool(*equals)(MessageInfo* msg1, MessageInfo* msg2))
{
#ifdef ON_LINUX
    Message::receive(dst);
    if (!(*equals)(dst, src))
    {
        fprintf(stderr, "not equal");
        exit(1);
    }

    return 0;
#else
    MessageInfo msg;
    for (int i = 0; ; i++)
    {
        int result = Message::peek(&msg, i);

        if (result != 0)
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
    return 0;
#endif
}

int Message::peek(MessageInfo* info, int index, int flags)
{
#ifdef ON_LINUX
    printf("%s not implemented\n", __func__);
    return 0;
#else
    return syscall_peek(info, index, flags);
#endif
}

void Message::create(MessageInfo* info, dword header, dword arg1 /*= 0*/, dword arg2 /*= 0*/, dword arg3 /*= 0*/, const char* str /*= NULL */)
{
    info->header = header;
    info->arg1 = arg1;
    info->arg2 = arg2;
    info->arg3 = arg3;

    if (str != NULL)
    {
//        strncpy(info->str, str, sizeof(info->str));
    memcpy(info->str, str, 128);
    }
    return;
}

bool Message::exist()
{
#ifdef ON_LINUX
    printf("%s not implemented\n", __func__);
    return 0;
#else
    return syscall_exist_message();
#endif
}

dword Message::lookup(const char* name)
{
#ifdef ON_LINUX
    printf("%s not implemented\n", __func__);
    return 0;
#else
    return syscall_lookup(name);
#endif
}

dword Message::lookupMainThread(const char* name)
{
#ifdef ON_LINUX
    return ProcessUtil::getPid(name);
#else
    return syscall_lookup_main_thread(name);
#endif
}

dword Message::lookupMainThread()
{
#ifdef ON_LINUX
    return getpid();
#else
    return syscall_lookup_main_thread(NULL);
#endif
}

}
