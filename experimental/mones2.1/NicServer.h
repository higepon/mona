#include <monapi.h>
#include <sys/types.h>
#include "Nic.h"
#include "NicFactory.h"
#pragma once

namespace mones {

#define MSG_FRAME_READY 0x12345678
#define MSG_FRAME_WRITE 0x87654321
#define MSG_FRAME_READ  0x87654322

class NicServer : public IPStack
{
public:
    NicServer();
    virtual ~NicServer();

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
