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
    return monapi_cmessage_receive(NULL, info);
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
    return monapi_cmessage_exist(NULL);
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
