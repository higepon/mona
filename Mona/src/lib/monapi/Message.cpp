/*----------------------------------------------------------------------
    Message
----------------------------------------------------------------------*/

#include <monapi/Message.h>
#include <monapi/cmessage.h>
#include <monapi/syscall.h>
#include <monapi/string.h>

namespace MonAPI {

int Message::send(dword tid, MessageInfo* info)
{
    return monapi_cmessage_send(tid, info);
}

int Message::send(dword tid, dword header, dword arg1 /*= 0*/, dword arg2 /*= 0*/, dword arg3 /*= 0*/, const char* str /*= NULL */)
{
    return monapi_cmessage_send_args(tid, header, arg1, arg2, arg3, str);
}

int Message::receive(MessageInfo* info)
{
    return monapi_cmessage_receive(info);
}

int Message::receive(MessageInfo* info, dword tid)
{
    return monapi_cmessage_receive_tid(info, tid);
}

int Message::receive(MessageInfo* info, dword tid, dword header)
{
    return monapi_cmessage_receive_header(info, tid, header);
}

int Message::receive(MessageInfo* info, dword tid, dword header, dword arg1)
{
    return monapi_cmessage_receive_arg1(info, tid, header, arg1);
}

int Message::sendReceive(MessageInfo* result, dword tid, MessageInfo* info)
{
    return monapi_cmessage_send_receive(result, tid, info);
}

int Message::sendReceive(MessageInfo* result, dword tid, dword header, dword arg1 /*= 0*/, dword arg2 /*= 0*/, dword arg3 /*= 0*/, const char* str /*= NULL */)
{
    return monapi_cmessage_send_receive_args(result, tid, header, arg1, arg2, arg3, str);
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
    return monapi_cmessage_exist();
}

dword Message::lookup(const char* name)
{
    return syscall_lookup(name);
}

dword Message::lookupMainThread(const char* name)
{
    return syscall_lookup_main_thread(name);
}

}
