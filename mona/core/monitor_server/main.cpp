/*!
  \file  Monitor
  \brief class Monitor

  Copyright (c) 2004 Higepon
  All rights reserved.
  License=MIT/X License

  \author  Higepon
  \version $Revision$
  \date   create:2004/06/19 update:$Date$
*/

#include <monapi.h>
#include <monapi/CString.h>
#include <servers/screen.h>
#include <monapi/messages.h>
#include <limits.h>
#include <map>
#include <string>

using namespace MonAPI;
using namespace std;;

#define CHECK_INTERVAL 5000

class Monitor
{
public:
    Monitor();
    virtual ~Monitor();

public:
    bool Initialize();
    void Service();

private:
    uint8_t* ReadConfig(CString file);
    void ParseConfig(CString content);
    void CheckServers();

private:
    HList<CString> paths;
    HList<CString> servers;
    bool firstLoad;
    bool* alive;

};

Monitor::Monitor() : firstLoad(true)
{
}

Monitor::~Monitor()
{

    delete alive;
}

void Monitor::Service()
{
    for (;;)
    {
    monapi_warn("%s %s:%d\n", __func__, __FILE__, __LINE__);
        CheckServers();
    monapi_warn("%s %s:%d\n", __func__, __FILE__, __LINE__);
        sleep(CHECK_INTERVAL);
    }
}

bool Monitor::Initialize()
{
    monapi_warn("%s %s:%d\n", __func__, __FILE__, __LINE__);
    scoped_ptr<SharedMemory> shm(monapi_file_read_all("/MONITOR.CFG"));
    monapi_warn("%s %s:%d\n", __func__, __FILE__, __LINE__);
    if (shm.get() == NULL)
    {
       MONAPI_WARN("Config file read error");
        return false;
    }
    ParseConfig((char*)(shm->data()));
//    shm->unmap();
    return true;
}

void Monitor::ParseConfig(CString content)
{
    _A<CString> lines = content.split_lines();

    FOREACH(CString, line, lines)
    {
        if (!line.startsWith("SERVER=")) continue;

        _A<CString> p = line.split("=");
        if (p.get_Length() < 2) continue;

        CString path = p[p.get_Length() - 1];

        _A<CString> q = path.split("/");

        CString name = q[q.get_Length() - 1];

        this->paths.add(path);
        this->servers.add(name);
    }
    END_FOREACH

    alive = new bool[servers.size()];

    return;
}

void Monitor::CheckServers()
{
    //char buf[256];
    PsInfo info;

    for (int i = 0; i < servers.size(); i++)
    {
        alive[i] = false;
    }

    syscall_set_ps_dump();

    while (syscall_read_ps_dump(&info) == 0)
    {
        CString name = info.name;

        for (int i = 0; i < servers.size(); i++)
        {
            if (servers.get(i) == name)
            {
                alive[i] = true;
                continue;
            }
        }
    }

    for (int i = 0; i < servers.size(); i++)
    {
        if (alive[i]) continue;
        if (!firstLoad && servers[i] == "OLDSHELL.EX5") continue;
        // sorry we can not use printf before process server starts.
        syscall_print("loading ");
        syscall_print((const char*)paths.get(i));
        syscall_print("....");

        // We need OutStream
        MessageInfo msg;
        if (servers[i] == "SCHEME.EX5")
        {
            uint32_t tid;
            uint32_t targetID = Message::lookupMainThread("SCREEN.EX5");
            if (targetID == THREAD_UNKNOWN || Message::sendReceive(&msg, targetID, MSG_SCREEN_GET_STREAM_HANDLE)) {
                syscall_print("SCREEN.EX5 not found\n");
                continue;
            }
            syscall_print(monapi_call_process_execute_file_get_tid((const char*)paths.get(i), MONAPI_FALSE, &tid, msg.arg2, msg.arg2) == 0? "OK\n" : "NG\n");
        }
        else
        {
            syscall_print(monapi_call_process_execute_file((const char*)paths.get(i), MONAPI_FALSE) == 0? "OK\n" : "NG\n");
        }

        for (;;)
        {
            if (Message::receive(&msg)) continue;
            if (msg.header == MSG_STARTED) break;
        }

    }

    if (firstLoad) firstLoad = false;
}

static volatile bool isNameServerStarted = false;

// Don't touch any files from this function, since it causes infinite loop on file server.
static void __fastcall nameServer(void* arg)
{
    isNameServerStarted = true;
    map<string, uint32_t> nameMap;
    for (;;) {
        MessageInfo msg;
        if (Message::receive(&msg) != M_OK) {
            continue;
        }
        switch (msg.header) {
        case MSG_NAME:
    monapi_warn("%s %s:%d\n", __func__, __FILE__, __LINE__);
            Message::reply(&msg, M_OK);
            break;
        case MSG_ADD:
    _logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
            nameMap[msg.str] = msg.from;
            if (Message::reply(&msg, M_OK) != M_OK) {
                monapi_warn("reply failed");
            }
    _logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
            break;
        case MSG_WHERE:
            map<string, uint32_t>::iterator it = nameMap.find(msg.str);
            if (it == nameMap.end()) {
                if (Message::reply(&msg, M_NAME_NOT_FOUND) != M_OK) {
                    monapi_warn("reply failed");
                }
            } else {
                if (Message::reply(&msg, M_OK, (*it).second) != M_OK) {
                    monapi_warn("reply failed");
                }
            }
            break;
        }
    }
    exit(0);
}

int main(int argc, char* argv[])
{
    syscall_mthread_create_with_arg(nameServer, NULL);
    monapi_warn("%s %s:%d\n", __func__, __FILE__, __LINE__);
    while (!isNameServerStarted );
    monapi_warn("%s %s:%d\n", __func__, __FILE__, __LINE__);

    Monitor monitor;
    monapi_warn("%s %s:%d\n", __func__, __FILE__, __LINE__);

    monitor.Initialize();
    monapi_warn("%s %s:%d\n", __func__, __FILE__, __LINE__);
    if (monapi_notify_server_start("INIT") != M_OK) {
        exit(-1);
    }
    monapi_warn("%s %s:%d\n", __func__, __FILE__, __LINE__);
    monitor.Service();
    monapi_warn("%s %s:%d\n", __func__, __FILE__, __LINE__);
    return 0;
}
