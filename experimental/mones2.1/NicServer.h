#include <monapi.h>
#include <sys/types.h>
#include "Nic.h"
#include "NicFactory.h"

#ifndef _MONA_MONES_NIC_SERVER_
#define _MON

namespace mones {

#define MSG_FRAME_READY 0x12345678
#define MSG_FRAME_WRITE 0x87654321
#define MSG_FRAME_READ 0x87654322
#define MSG_GET_MAC_ADDRESS 0x41530000

typedef struct {
    byte destmac[6];
    byte header[4096];
    dword size;
    word protocol;
} OutPacket;

void SetFrameToSharedMemory(Ether* frame);
void GetFrameFromSharedMemory(Ether* frame);

class NicServer
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
    void interrupt(MessageInfo* msg);
    void dumpPacket(Ether*);
    void ICMPreply(Ether*);
protected:
    byte macAddress[6];
    dword observerThread;
    dword myID;
    Nic* nic;
    bool started;
    bool loopExit;
};
}; // namespace mones
#endif
