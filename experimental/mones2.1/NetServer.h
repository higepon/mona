#include <monapi.h>
#include <sys/types.h>
#include "Nic.h"
#include "NicFactory.h"
#pragma once

namespace mones {

#define MSG_NET_STATUS 0xBACA0001
#define MSG_NET_OPEN   0xBACA0002
#define MSG_NET_CLOSE  0xBACA0003
#define MSG_NET_WRITE  0xBACA0004
#define MSG_NET_READ   0xBACA0005

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
