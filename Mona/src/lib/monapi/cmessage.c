/*----------------------------------------------------------------------
    Message for C
----------------------------------------------------------------------*/

#include <monapi/cmessage.h>
#include <monapi/string.h>
#include <monapi/syscall.h>

#define ASSERT(cond) if (!cond) { printf("%s:%d: null pointer exception!\n", __FILE__, __LINE__); exit(1); }

static monapi_clist msg_queue;  /* for main thread */

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

int monapi_cmessage_receive(monapi_clist* queue, MessageInfo* dst)
{
    int result;
    if (queue == NULL) queue = &msg_queue;

    if (queue->count > 0)
    {
        MessageInfo* msg = (MessageInfo*)monapi_clist_remove_at(queue, 0);
        *dst = *msg;
        free(msg);
        return 0;
    }

    result = syscall_receive(dst);
    if (result != 0)
    {
         syscall_mthread_yield_message();
         result = syscall_receive(dst);
    }
    return result;
}

int monapi_cmessage_receive_cond(monapi_clist* queue, MessageInfo* dst, MessageInfo* src, MONAPI_BOOL(*cond)(MessageInfo*, MessageInfo*))
{
    int i;
    if (queue == NULL) queue = &msg_queue;

    for (i = 0; i < queue->count; i++)
    {
        MessageInfo* msg = (MessageInfo*)monapi_clist_get_item(queue, i);
        if ((*cond)(msg, src))
        {
            *dst = *msg;
            monapi_clist_remove_at(queue, i);
            free(msg);
            return 0;
        }
    }

    for (;;)
    {
        MessageInfo* msg;
        int result = syscall_receive(dst);
        if (result != 0)
        {
             syscall_mthread_yield_message();
             result = syscall_receive(dst);
        }
        if (result != 0) continue;
        if ((*cond)(dst, src)) break;

        msg = (MessageInfo*)malloc(sizeof(MessageInfo));
        ASSERT(msg);
        *msg = *dst;
        monapi_clist_add(queue, msg);
    }
    return 0;
}

int monapi_cmessage_send_receive(monapi_clist* queue, MessageInfo* dst, dword tid, MessageInfo* info)
{
    MessageInfo src;
    int ret = monapi_cmessage_send(tid, info);
    if (ret != 0) return ret;

    if (queue == NULL) queue = &msg_queue;

    src.from = tid;
    src.header = MSG_RESULT_OK;
    src.arg1 = info->header;

    if (dst == NULL)
    {
        MessageInfo msg;
        return monapi_cmessage_receive_cond(queue, &msg, &src, monapi_cmessage_cond_from_header_arg1);
    }
    return monapi_cmessage_receive_cond(queue, dst, &src, monapi_cmessage_cond_from_header_arg1);
}

int monapi_cmessage_send_receive_args(monapi_clist* queue, MessageInfo* dst, dword tid, dword header, dword arg1, dword arg2, dword arg3, const char* str)
{
    MessageInfo src;
    int ret = monapi_cmessage_send_args(tid, header, arg1, arg2, arg3, str);
    if (ret != 0) return ret;

    if (queue == NULL) queue = &msg_queue;

    src.from = tid;
    src.header = MSG_RESULT_OK;
    src.arg1 = header;

    if (dst == NULL)
    {
        MessageInfo msg;
        return monapi_cmessage_receive_cond(queue, &msg, &src, monapi_cmessage_cond_from_header_arg1);
    }
    return monapi_cmessage_receive_cond(queue, dst, &src, monapi_cmessage_cond_from_header_arg1);
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

int monapi_cmessage_exist(monapi_clist* queue)
{
    if (queue == NULL) queue = &msg_queue;
    if (queue->count > 0) return 1;
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

MONAPI_BOOL monapi_cmessage_cond_from(MessageInfo* msg1, MessageInfo* msg2)
{
    return msg1->from == msg2->from ? MONAPI_TRUE : MONAPI_FALSE;
}

MONAPI_BOOL monapi_cmessage_cond_from_header(MessageInfo* msg1, MessageInfo* msg2)
{
    return msg1->from == msg2->from && msg1->header == msg2->header ? MONAPI_TRUE : MONAPI_FALSE;
}

MONAPI_BOOL monapi_cmessage_cond_from_header_arg1(MessageInfo* msg1, MessageInfo* msg2)
{
    return msg1->from == msg2->from && msg1->header == msg2->header && msg1->arg1 == msg2->arg1 ? MONAPI_TRUE : MONAPI_FALSE;
}

MONAPI_BOOL monapi_cmessage_cond_header(MessageInfo* msg1, MessageInfo* msg2)
{
    return msg1->header == msg2->header ? MONAPI_TRUE : MONAPI_FALSE;
}
