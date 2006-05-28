//$Id$
#include <monapi.h>
#include <sys/types.h>
#include "Net.h"
#include "Nic.h"
#include "NicFactory.h"
#pragma once

namespace mones {

class NetServer
{
public:
    NetServer();
    virtual ~NetServer();
    bool initialize();
    dword getThreadID() const;
    void messageLoop();
    bool isStarted() {return started;}
    void exit();
private:
    struct ConnectionInfo{
        struct{
            dword remoteip;
            word  localport;
            word  remoteport;
            word  protocol;
        } Id;
        dword clientid;
        word  netdsc;
        byte  status;
        MessageInfo msg;
    };
    MonAPI::Mutex mutex;
    IPStack* ipstack;
    void ICMPreply(IP*);
    void Dispatch();
    void getfreeport(MessageInfo* msg);
    void interrupt(MessageInfo* msg);
    void open(MessageInfo* msg);
    void read(MessageInfo* msg);
    void write(MessageInfo* msg);
    void close(MessageInfo* msg);
    void status(MessageInfo* msg);
    void ontimer(MessageInfo* msg);
    void config(MessageInfo* msg);
    word next_port;
    HList<ConnectionInfo*> cinfolist;

protected:
    byte macAddress[6];
    dword observerThread;
    dword myID;
    dword timerid;
    Nic* nic;
    bool started;
    bool loopExit;
};
}; // namespace mones
