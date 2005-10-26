/*----------------------------------------------------------------------
    Message
----------------------------------------------------------------------*/

#include <monapi/Message.h>
#include <monapi/syscall.h>
#include <monapi/string.h>

namespace MonAPI {

int Message::send(dword tid, MessageInfo* info)
{
    if (tid == THREAD_UNKNOWN) return 1;
    return syscall_send(tid, info);
}

int Message::send(dword tid, dword header, dword arg1 /*= 0*/, dword arg2 /*= 0*/, dword arg3 /*= 0*/, const char* str /*= NULL */)
{
    MessageInfo info;
    Message::create(&info, header, arg1, arg2, arg3, str);
    return Message::send(tid, &info);
}

int Message::receive(MessageInfo* info)
{
    int result = syscall_receive(info);
    if (result != 0)
    {
         syscall_mthread_yield_message();
         result = syscall_receive(info);
    }
    return result;
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
    return Message::send(info->from, MSG_RESULT_OK, info->header, arg2, arg3, str);
}

int Message::receive(MessageInfo* dst, MessageInfo* src, bool(*equals)(MessageInfo* msg1, MessageInfo* msg2))
{
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
}

int Message::peek(MessageInfo* info, int index, int flags)
{
    return syscall_peek(info, index, flags);
}

void Message::create(MessageInfo* info, dword header, dword arg1 /*= 0*/, dword arg2 /*= 0*/, dword arg3 /*= 0*/, const char* str /*= NULL */)
{
    info->header = header;
    info->arg1 = arg1;
    info->arg2 = arg2;
    info->arg3 = arg3;

    if (str != NULL)
    {
        strncpy(info->str, str, sizeof(info->str));
    }
    return;
}

bool Message::exist()
{
    return syscall_exist_message();
}

dword Message::lookup(const char* name)
{
    return syscall_lookup(name);
}

dword Message::lookupMainThread(const char* name)
{
    return syscall_lookup_main_thread(name);
}

dword Message::lookupMainThread()
{
    return syscall_lookup_main_thread(NULL);
}

}
