/*----------------------------------------------------------------------
    Message
----------------------------------------------------------------------*/

#include <monapi.h>
#include <monapi/clist.h>

static monapi_clist msg_queue;

namespace MonAPI {

int Message::send(dword tid, MessageInfo* info)
{
    if (tid == 0xffffffff) return 1;
    return syscall_send(tid, info);
}

int Message::send(dword tid, dword header, dword arg1 /*= 0*/, dword arg2 /*= 0*/, dword arg3 /*= 0*/, const char* str /*= NULL */)
{
    MessageInfo info;
    create(&info, header, arg1, arg2, arg3, str);
    return send(tid, &info);
}

int Message::receive(MessageInfo* info)
{
    if (msg_queue.count > 0)
    {
        MessageInfo* msg = (MessageInfo*)monapi_clist_remove_at(&msg_queue, 0);
        *info = *msg;
        delete msg;
        return 0;
    }

    int result = syscall_receive(info);
    if (result != 0)
    {
         syscall_mthread_yeild_message();
         result = syscall_receive(info);
    }
    return result;
}

int Message::receive(MessageInfo* info, dword tid)
{
    int size = msg_queue.count;
    for (int i = 0; i < size; i++)
    {
        MessageInfo* msg = (MessageInfo*)monapi_clist_get_item(&msg_queue, i);
        if (msg->from == tid)
        {
            *info = *msg;
            monapi_clist_remove_at(&msg_queue, i);
            delete msg;
            return 0;
        }
    }

    for (;;)
    {
        int result = syscall_receive(info);
        if (result != 0)
        {
             syscall_mthread_yeild_message();
             result = syscall_receive(info);
        }
        if (result != 0) continue;
        if (info->from == tid) break;
        monapi_clist_add(&msg_queue, new MessageInfo(*info));
    }
    return 0;
}

int Message::receive(MessageInfo* info, dword tid, dword header)
{
    int size = msg_queue.count;
    for (int i = 0; i < size; i++)
    {
        MessageInfo* msg = (MessageInfo*)monapi_clist_get_item(&msg_queue, i);
        if (msg->from == tid && msg->header == header)
        {
            *info = *msg;
            monapi_clist_remove_at(&msg_queue, i);
            delete msg;
            return 0;
        }
    }

    for (;;)
    {
        int result = syscall_receive(info);
        if (result != 0)
        {
             syscall_mthread_yeild_message();
             result = syscall_receive(info);
        }
        if (result != 0) continue;
        if (info->from == tid && info->header == header) break;
        monapi_clist_add(&msg_queue, new MessageInfo(*info));
    }
    return 0;
}

int Message::receive(MessageInfo* info, dword tid, dword header, dword arg1)
{
    int size = msg_queue.count;
    for (int i = 0; i < size; i++)
    {
        MessageInfo* msg = (MessageInfo*)monapi_clist_get_item(&msg_queue, i);
        if (msg->from == tid && msg->header == header && msg->arg1 == arg1)
        {
            *info = *msg;
            monapi_clist_remove_at(&msg_queue, i);
            delete msg;
            return 0;
        }
    }

    for (;;)
    {
        int result = syscall_receive(info);
        if (result != 0)
        {
             syscall_mthread_yeild_message();
             result = syscall_receive(info);
        }
        if (result != 0) continue;
        if (info->from == tid && info->header == header && info->arg1 == arg1) break;
        monapi_clist_add(&msg_queue, new MessageInfo(*info));
    }
    return 0;
}

int Message::sendReceive(MessageInfo* result, dword tid, MessageInfo* info)
{
    int ret = send(tid, info);
    if (ret != 0) return ret;

    if (result == NULL)
    {
        MessageInfo msg;
        return receive(&msg, tid, MSG_RESULT_OK, info->header);
    }
    return receive(result, tid, MSG_RESULT_OK, info->header);
}

int Message::sendReceive(MessageInfo* result, dword tid, dword header, dword arg1 /*= 0*/, dword arg2 /*= 0*/, dword arg3 /*= 0*/, const char* str /*= NULL */)
{
    int ret = send(tid, header, arg1, arg2, arg3, str);
    if (ret != 0) return ret;

    if (result == NULL)
    {
        MessageInfo msg;
        return receive(&msg, tid, MSG_RESULT_OK, header);
    }
    return receive(result, tid, MSG_RESULT_OK, header);
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
    if (msg_queue.count > 0) return true;
    return (syscall_exist_message() == 1);
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
