#include "ElfServer.h"
#include "ProcessManager.h"

#include <monapi.h>
#include <monapi/messages.h>

using namespace MonAPI;

static HList<ProcessInfo> infos;
static HList<dword> receivers;

ProcessInfo getProcessInfo(dword tid)
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
        infos.add(pi);
    }
}

void addProcessInfo(dword tid, dword parent, const CString& path)
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
    infos.add(pi);
    notifyProcessChanged(MSG_PROCESS_CREATED, tid, parent, path);
}

dword addProcessInfo(dword parent, const CString& name, const CString& path)
{
    dword ret = THREAD_UNKNOWN;
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
        infos.add(pi);
        ret = info.tid;
    }
    return ret;
}

void addProcessInfo(dword tid, dword parent, const CString& name, const CString& path)
{
    ProcessInfo pi(tid, parent, name, path);
    infos.add(pi);
}

void removeProcessInfo(dword tid)
{
    int size = infos.size();
    for (int i = 0; i < size; i++)
    {
        if (infos[i].tid != tid) continue;

        infos.removeAt(i);
        notifyProcessChanged(MSG_PROCESS_TERMINATED, tid);
        return;
    }
}

static void registerReceiver(dword tid)
{
    int size = receivers.size();
    for (int i = 0; i < size; i++)
    {
        if (receivers[i] == tid) return;
    }
    receivers.add(tid);
}

static void unregisterReceiver(dword tid)
{
    int size = receivers.size();
    for (int i = 0; i < size; i++)
    {
        if (receivers[i] != tid) continue;

        receivers.removeAt(i);
        return;
    }
}

void notifyProcessChanged(dword header, dword tid, dword parent /*= 0*/, const CString& path /*= NULL*/)
{
    int i = 0;
    while (i < receivers.size())
    {
        if (Message::send(receivers[i], header, tid, parent, 0, path) == 0)
        {
            i++;
        }
        else
        {
            printf("%s: can not connect to %d\n", SVR, receivers[i]);
            receivers.removeAt(i);
        }
    }
}

bool handleMessage(MessageInfo msg)
{
    switch (msg.header)
    {
        case MSG_REGISTER_TO_SERVER:
            registerReceiver(msg.arg1);
            Message::reply(&msg);
            break;
        case MSG_UNREGISTER_FROM_SERVER:
            unregisterReceiver(msg.arg1);
            Message::reply(&msg);
            break;
        case MSG_PROCESS_GET_PROCESS_INFO:
        {
            ProcessInfo pi = getProcessInfo(msg.from);
            Message::reply(&msg, pi.parent, 0, pi.path);
            break;
        }
        case MSG_PROCESS_CREATED:
            addProcessInfo(msg.arg1, msg.arg2, msg.str);
            break;
        case MSG_PROCESS_TERMINATED:
            removeProcessInfo(msg.arg1);
            break;
        default:
            return false;
    }
    return true;
}
