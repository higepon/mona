/*----------------------------------------------------------------------
    Message for C
----------------------------------------------------------------------*/

#include <monapi/cmessage.h>
#include <monapi/clist.h>
#include <monapi/string.h>
#include <monapi/syscall.h>

#define ASSERT(cond) if (!cond) { printf("%s:%d: null pointer exception!\n", __FILE__, __LINE__); exit(1); }

static monapi_clist msg_queue;

int monapi_cmessage_send(dword tid, MessageInfo* info)
{
    if (tid == THREAD_UNKNOWN) return 1;
    return syscall_send(tid, info);
}

int monapi_cmessage_send_args(dword tid, dword header, dword arg1, dword arg2, dword arg3, const char* str)
{
    MessageInfo info;
    monapi_cmessage_create(&info, header, arg1, arg2, arg3, str);
    return monapi_cmessage_send(tid, &info);
}

int monapi_cmessage_receive(MessageInfo* info)
{
    int result;

    if (msg_queue.count > 0)
    {
        MessageInfo* msg = (MessageInfo*)monapi_clist_remove_at(&msg_queue, 0);
        *info = *msg;
        free(msg);
        return 0;
    }

    result = syscall_receive(info);
    if (result != 0)
    {
         syscall_mthread_yeild_message();
         result = syscall_receive(info);
    }
    return result;
}

int monapi_cmessage_receive_tid(MessageInfo* info, dword tid)
{
    int i;
    for (i = 0; i < msg_queue.count; i++)
    {
        MessageInfo* msg = (MessageInfo*)monapi_clist_get_item(&msg_queue, i);
        if (msg->from == tid)
        {
            *info = *msg;
            monapi_clist_remove_at(&msg_queue, i);
            free(msg);
            return 0;
        }
    }

    for (;;)
    {
        MessageInfo* mi;
        int result = syscall_receive(info);
        if (result != 0)
        {
             syscall_mthread_yeild_message();
             result = syscall_receive(info);
        }
        if (result != 0) continue;
        if (info->from == tid) break;

        mi = (MessageInfo*)malloc(sizeof(MessageInfo));
        ASSERT(mi);
        *mi = *info;
        monapi_clist_add(&msg_queue, mi);
    }
    return 0;
}

int monapi_cmessage_receive_header(MessageInfo* info, dword tid, dword header)
{
    int i;
    for (i = 0; i < msg_queue.count; i++)
    {
        MessageInfo* msg = (MessageInfo*)monapi_clist_get_item(&msg_queue, i);
        if (msg->from == tid && msg->header == header)
        {
            *info = *msg;
            monapi_clist_remove_at(&msg_queue, i);
            free(msg);
            return 0;
        }
    }

    for (;;)
    {
        MessageInfo* mi;
        int result = syscall_receive(info);
        if (result != 0)
        {
             syscall_mthread_yeild_message();
             result = syscall_receive(info);
        }
        if (result != 0) continue;
        if (info->from == tid && info->header == header) break;

        mi = (MessageInfo*)malloc(sizeof(MessageInfo));
        ASSERT(mi);
        *mi = *info;
        monapi_clist_add(&msg_queue, mi);
    }
    return 0;
}

int monapi_cmessage_receive_arg1(MessageInfo* info, dword tid, dword header, dword arg1)
{
    int i;
    for (i = 0; i < msg_queue.count; i++)
    {
        MessageInfo* msg = (MessageInfo*)monapi_clist_get_item(&msg_queue, i);
        if (msg->from == tid && msg->header == header && msg->arg1 == arg1)
        {
            *info = *msg;
            monapi_clist_remove_at(&msg_queue, i);
            free(msg);
            return 0;
        }
    }

    for (;;)
    {
        MessageInfo* mi;
        int result = syscall_receive(info);
        if (result != 0)
        {
             syscall_mthread_yeild_message();
             result = syscall_receive(info);
        }
        if (result != 0) continue;
        if (info->from == tid && info->header == header && info->arg1 == arg1) break;

        mi = (MessageInfo*)malloc(sizeof(MessageInfo));
        ASSERT(mi);
        *mi = *info;
        monapi_clist_add(&msg_queue, mi);
    }
    return 0;
}

int monapi_cmessage_receive_header_only(MessageInfo* info, dword header)
{
    int i;
    for (i = 0; i < msg_queue.count; i++)
    {
        MessageInfo* msg = (MessageInfo*)monapi_clist_get_item(&msg_queue, i);
        if (msg->header == header)
        {
            *info = *msg;
            monapi_clist_remove_at(&msg_queue, i);
            free(msg);
            return 0;
        }
    }

    for (;;)
    {
        MessageInfo* mi;
        int result = syscall_receive(info);
        if (result != 0)
        {
             syscall_mthread_yeild_message();
             result = syscall_receive(info);
        }
        if (result != 0) continue;
        if (info->header == header) break;

        mi = (MessageInfo*)malloc(sizeof(MessageInfo));
        ASSERT(mi);
        *mi = *info;
        monapi_clist_add(&msg_queue, mi);
    }
    return 0;
}

int monapi_cmessage_send_receive(MessageInfo* result, dword tid, MessageInfo* info)
{
    int ret = monapi_cmessage_send(tid, info);
    if (ret != 0) return ret;

    if (result == NULL)
    {
        MessageInfo msg;
        return monapi_cmessage_receive_arg1(&msg, tid, MSG_RESULT_OK, info->header);
    }
    return monapi_cmessage_receive_arg1(result, tid, MSG_RESULT_OK, info->header);
}

int monapi_cmessage_send_receive_args(MessageInfo* result, dword tid, dword header, dword arg1, dword arg2, dword arg3, const char* str)
{
    int ret = monapi_cmessage_send_args(tid, header, arg1, arg2, arg3, str);
    if (ret != 0) return ret;

    if (result == NULL)
    {
        MessageInfo msg;
        return monapi_cmessage_receive_arg1(&msg, tid, MSG_RESULT_OK, header);
    }
    return monapi_cmessage_receive_arg1(result, tid, MSG_RESULT_OK, header);
}

int monapi_cmessage_reply(MessageInfo* info)
{
    return monapi_cmessage_send_args(info->from, MSG_RESULT_OK, info->header, 0, 0, NULL);
}

int monapi_cmessage_reply_args(MessageInfo* info, dword arg2, dword arg3, const char* str)
{
    return monapi_cmessage_send_args(info->from, MSG_RESULT_OK, info->header, arg2, arg3, str);
}

void monapi_cmessage_create(MessageInfo* info, dword header, dword arg1, dword arg2, dword arg3, const char* str)
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

int monapi_cmessage_exist()
{
    if (msg_queue.count > 0) return 1;
    return (syscall_exist_message() == 1);
}

dword monapi_cmessage_lookup(const char* name)
{
    return syscall_lookup(name);
}

dword monapi_cmessage_lookup_main_thread(const char* name)
{
    return syscall_lookup_main_thread(name);
}
