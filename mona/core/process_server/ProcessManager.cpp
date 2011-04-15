#include "ProcessServer.h"
#include "ProcessManager.h"

#include <monapi.h>
#include <monapi/messages.h>
#include <monapi/Assert.h>
#include <vector>

using namespace MonAPI;
using namespace std;

static SharedMemory* commonParams = NULL;
static vector<ProcessInfo> infos;
static HList<uint32_t> receivers;

void initCommonParameters()
{
    commonParams = new SharedMemory(sizeof(CommonParameters));
    if (commonParams->map(false) != M_OK) {
        exit(1);
    }
}

ProcessInfo getProcessInfo(uint32_t tid)
{
    int size = infos.size();
    for (int i = 0; i < size; i++)
    {
        if (infos[i].tid == tid) {
            return infos[i];
        }
    }
    // todo tid not required
    return ProcessInfo(tid);
}

intptr_t addProcessInfo(uint32_t parentTid, uint32_t subThreadTid)
{
    ProcessInfo parent = getProcessInfo(parentTid);
    if (parent.tid == THREAD_UNKNOWN) {
        return M_NOT_FOUND;
    }
    // todo copy constructor
    _printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    infos.push_back(ProcessInfo(subThreadTid, parent.tid, parent.name, parent.path, parent.stdin_id, parent.stdin_id));
    return M_OK;
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
        if (Message::send(receivers[i], MSG_PROCESS_CREATED, tid, parent, 0, path) == M_OK)
        {
            i++;
        }
        else
        {
            _printf("%s: can not connect to %d\n", SVR, receivers[i]);
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
        if (Message::send(receivers[i], MSG_PROCESS_TERMINATED, tid, status) == M_OK)
        {
            i++;
        }
        else
        {
            _printf("%s: can not connect to %d\n", SVR, receivers[i]);
            removeProcessInfo(receivers[i]);
            receivers.removeAt(i);
        }
    }
}


bool processHandler(MessageInfo* msg)
{
    switch (msg->header)
    {
        case MSG_PROCESS_GET_PROCESS_STDIO:
        {
            ProcessInfo pi = getProcessInfo(msg->arg1);
            Message::reply(msg, pi.stdin_id, pi.stdout_id);
            break;
        }
        case MSG_PROCESS_TERMINATED:
            removeProcessInfo(msg->arg1, msg->arg2);
            break;
        case MSG_PROCESS_GET_COMMON_PARAMS:
            Message::reply(msg, commonParams->handle());
            break;
        case MSG_PROCESS_REGISTER_THREAD:
        {
            uint32_t parentTid = msg->from;
            uint32_t subThreadTid = msg->arg1;
            intptr_t ret = addProcessInfo(parentTid, subThreadTid);
            Message::reply(msg, ret);
            break;
        }
        default:
            return false;
    }
    return true;
}
