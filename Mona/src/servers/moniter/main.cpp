/*!
  \file  Monitor
  \brief class Monitor

  Copyright (c) 2004 Higepon
  All rights reserved.
  License=MIT/X Licnese

  \author  HigePon
  \version $Revision$
  \date   create:2004/06/19 update:$Date$
*/

#include <monapi.h>
#include <monapi/CString.h>
#include <monapi/messages.h>

using namespace MonAPI;

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
    byte* ReadConfig(CString file);
    void ParseConfig(CString content);
    void CheckServers();

private:
    HList<CString> paths;
    HList<CString> servers;
    bool* alive;

};

Monitor::Monitor()
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
        CheckServers();
        sleep(CHECK_INTERVAL);
    }
}

bool Monitor::Initialize()
{
    char* config;

    config = (char*)ReadConfig("/MONITOR.CFG");
    if (config == NULL)
    {
        printf("Config file read error\n");
        return false;
    }

    ParseConfig(config);

#if 0
    for (int i = 0; i < servers.size(); i++)
    {
        printf("[%s]\n", (const char*)servers.get(i));
    }
#endif

    delete[] config;
    return true;
}

void Monitor::ParseConfig(CString content)
{
    _A<CString> lines = content.split("\r\n");

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

byte* Monitor::ReadConfig(CString file)
{
    int result;
    int fileSize;
    byte* buf;

    FileInputStream fis(file);

    result = fis.open();

    if (result != 0) return NULL;

    fileSize = fis.getFileSize();
    buf = new byte[fileSize];

    if (buf == NULL)
    {
        fis.close();
        return NULL;
    }

    if (fis.read(buf, fileSize))
    {
        fis.close();
        return NULL;
    }

    fis.close();
    return buf;
}

void Monitor::CheckServers()
{
    char buf[256];
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
                break;
            }
        }
    }

    for (int i = 0; i < servers.size(); i++)
    {
        if (alive[i]) continue;

        printf("loading %s....", (const char*)paths.get(i));

        printf("%s\n", monapi_call_process_execute_file((const char*)paths.get(i), MONAPI_FALSE) == 0? "OK" : "NG");
        MessageInfo msg;
        for (;;)
        {
            if (Message::receive(&msg)) continue;
            if (msg.header == MSG_SERVER_START_OK) break;
        }

    }
}

int MonaMain(List<char*>* pekoe)
{
    Monitor monitor;

    monitor.Initialize();

    /* Server start ok */
    dword targetID = Message::lookupMainThread("INIT");

    if(targetID == 0xFFFFFFFF)
    {
        printf("ShellServer:INIT not found\n");
        exit(1);
    }

    /* send */
    if(Message::send(targetID, MSG_SERVER_START_OK))
    {
        printf("ShellServer:INIT error\n");
    }

    monitor.Service();

    return 0;
}
