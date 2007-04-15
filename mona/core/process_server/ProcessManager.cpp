#include "ProcessServer.h"
#include "ProcessManager.h"

#include <monapi.h>
#include <monapi/messages.h>
#include <monapi/Assert.h>
#include <vector>

using namespace MonAPI;
using namespace std;

static monapi_cmemoryinfo* commonParams = NULL;
static vector<ProcessInfo> infos;
static HList<uint32_t> receivers;

void initCommonParameters()
{
    commonParams = monapi_cmemoryinfo_new();
    if (!monapi_cmemoryinfo_create(commonParams, sizeof(CommonParameters), 1)) exit(1);
}

ProcessInfo getProcessInfo(uint32_t tid)
{
    int size = infos.size();
    for (int i = 0; i < size; i++)
    {
        if (infos[i].tid == tid) return infos[i];
    }
    return ProcessInfo();
}

void addProcessInfo(const CString& name)
{
    syscall_set_ps_dump();
    PsInfo info;

    while (syscall_read_ps_dump(&info) == 0)
    {
        if (name != info.name) continue;

        ProcessInfo pi = getProcessInfo(info.tid);
        if (pi.tid != THREAD_UNKNOWN) continue;

        pi.tid  = info.tid;
        pi.name = info.name;
        infos.push_back(pi);
    }
}

void addProcessInfo(uint32_t tid, uint32_t parent, const CString& path)
{
    ProcessInfo pi = getProcessInfo(tid);
    if (pi.tid != THREAD_UNKNOWN) return;

    syscall_set_ps_dump();
    PsInfo info, found;
    found.tid = THREAD_UNKNOWN;

    while (syscall_read_ps_dump(&info) == 0)
    {
        if (found.tid == THREAD_UNKNOWN && info.tid == tid) found = info;
    }
    if (found.tid == THREAD_UNKNOWN) return;

    pi.tid    = tid;
    pi.parent = parent;
    pi.name   = found.name;
    pi.path   = path;
    infos.push_back(pi);
    notifyProcessCreated(tid, parent, path);
}

uint32_t addProcessInfo(uint32_t parent, const CString& name, const CString& path, uint32_t stdin_id, uint32_t stdout_id)
{
    uint32_t ret = THREAD_UNKNOWN;
    syscall_set_ps_dump();
    PsInfo info;

    while (syscall_read_ps_dump(&info) == 0)
    {
        if (ret != THREAD_UNKNOWN || name != info.name) continue;

        ProcessInfo pi = getProcessInfo(info.tid);
        if (pi.tid != THREAD_UNKNOWN) continue;

        pi.tid    = info.tid;
        pi.parent = parent;
        pi.name   = name;
        pi.path   = path;
        pi.stdin_id = stdin_id;
        pi.stdout_id = stdout_id;
        infos.push_back(pi);
        ret = info.tid;
    }
    return ret;
}

void addProcessInfo(uint32_t tid, uint32_t parent, const CString& name, const CString& path, uint32_t stdin_id, uint32_t stdout_id)
{
    ProcessInfo pi(tid, parent, name, path, stdin_id, stdout_id);
    infos.push_back(pi);
}

void removeProcessInfo(uint32_t tid, int status /* = -1 */)
{
    int size = infos.size();
    for (int i = 0; i < size; i++)
    {
        if (infos[i].tid != tid) continue;

        infos.erase(&infos[i]);
        notifyProcessTerminated(tid, status);
        return;
    }
}

static void registerReceiver(uint32_t tid)
{
    int size = receivers.size();
    for (int i = 0; i < size; i++)
    {
        if (receivers[i] == tid) return;
    }
    receivers.add(tid);
}

static void unregisterReceiver(uint32_t tid)
{
    int size = receivers.size();
    for (int i = 0; i < size; i++)
    {
        if (receivers[i] != tid) continue;
        receivers.removeAt(i);
        return;
    }
}

void notifyProcessCreated(uint32_t tid, uint32_t parent, const CString& path)
{
    int i = 0;
    while (i < receivers.size())
    {
        if (Message::send(receivers[i], MSG_PROCESS_CREATED, tid, parent, 0, path) == 0)
        {
            i++;
        }
        else
        {
            printf("%s: can not connect to %d\n", SVR, receivers[i]);
            removeProcessInfo(receivers[i]);
            receivers.removeAt(i);
        }
    }
}

void notifyProcessTerminated(uint32_t tid, int status)
{
    int i = 0;
    while (i < receivers.size())
    {
        if (Message::send(receivers[i], MSG_PROCESS_TERMINATED, tid, status) == 0)
        {
            i++;
        }
        else
        {
            printf("%s: can not connect to %d\n", SVR, receivers[i]);
            removeProcessInfo(receivers[i]);
            receivers.removeAt(i);
        }
    }
}


bool processHandler(MessageInfo* msg)
{
    switch (msg->header)
    {
        case MSG_REGISTER_TO_SERVER:
            registerReceiver(msg->arg1);
            Message::reply(msg);
            break;
        case MSG_UNREGISTER_FROM_SERVER:
            unregisterReceiver(msg->arg1);
            Message::reply(msg);
            break;
        case MSG_PROCESS_GET_PROCESS_INFO:
        {
            ProcessInfo pi = getProcessInfo(msg->from);
            Message::reply(msg, pi.parent, pi.stdout_id, pi.path);
            break;
        }
        case MSG_PROCESS_GET_PROCESS_STDIO:
        {
            ProcessInfo pi = getProcessInfo(msg->from);
            Message::reply(msg, pi.stdin_id, pi.stdout_id);
            break;
        }
        case MSG_PROCESS_CREATED:
            addProcessInfo(msg->arg1, msg->arg2, msg->str);
            break;
        case MSG_PROCESS_TERMINATED:
            removeProcessInfo(msg->arg1, msg->arg2);
            break;
        case MSG_PROCESS_GET_COMMON_PARAMS:
            Message::reply(msg, commonParams->Handle);
            break;
        default:
            return false;
    }
    return true;
}
