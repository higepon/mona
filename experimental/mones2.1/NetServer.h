#include <monapi.h>
#include <sys/types.h>
#include "Net.h"
#include "Nic.h"
#include "NicFactory.h"
#pragma once

namespace mones {

class NetServer : public IPStack
{
public:
    NetServer();
    virtual ~NetServer();

public:
    bool initialize();
    dword getThreadID() const;
    void messageLoop();
    bool isStarted() {return started;}
    void exit();

private:
    void ICMPreply(IP*);
    void interrupt(MessageInfo* msg);
protected:
    byte macAddress[6];
    dword observerThread;
    dword myID;
    Nic* nic;
    bool started;
    bool loopExit;
};
}; // namespace mones
