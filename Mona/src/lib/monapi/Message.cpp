/*----------------------------------------------------------------------
    Message
----------------------------------------------------------------------*/

#include <monapi.h>

namespace MonAPI {

HList<MessageInfo*>* Message::queue;

void Message::initialize()
{
    queue = new HList<MessageInfo*>();
}

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
	if (queue->size() > 0)
	{
        MessageInfo* msg = queue->removeAt(0);
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
    int size = queue->size();
    for (int i = 0; i < size; i++)
    {
        MessageInfo* msg = queue->get(i);
        if (msg->from == tid)
        {
            *info = *msg;
            queue->removeAt(i);
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
        queue->add(new MessageInfo(*info));
    }
    return 0;
}

int Message::receive(MessageInfo* info, dword tid, dword header)
{
    int size = queue->size();
    for (int i = 0; i < size; i++)
    {
        MessageInfo* msg = queue->get(i);
        if (msg->from == tid && msg->header == header)
        {
            *info = *msg;
            queue->removeAt(i);
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
        queue->add(new MessageInfo(*info));
    }
    return 0;
}

int Message::receive(MessageInfo* info, dword tid, dword header, dword arg1)
{
    int size = queue->size();
    for (int i = 0; i < size; i++)
    {
        MessageInfo* msg = queue->get(i);
        if (msg->from == tid && msg->header == header && msg->arg1 == arg1)
        {
            *info = *msg;
            queue->removeAt(i);
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
		MessageInfo* mi = new MessageInfo(*info);
        queue->add(mi);
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
    if (queue->size() > 0) return true;
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
