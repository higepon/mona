//$Id$
#pragma once
#include "Net.h"
#include "IPStack.h"

namespace mones {

class NetServer: public IPStack
{
public:
    NetServer();
    virtual ~NetServer();
    dword getThreadID() const;
    void messageLoop();
    bool isStarted() {return started;}
    void exit();
private:
    MonAPI::Mutex mutex;
    void Dispatch();
    void getfreeport(MessageInfo* msg);
    void Interrupt(MessageInfo* msg);
    void open(MessageInfo* msg);
    void read_bottom_half(int,ConnectionInfo*);
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
    dword myID;
    dword timerid;
    bool started;
    bool loopExit;
};
}; // namespace mones
