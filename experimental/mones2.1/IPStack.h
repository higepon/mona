//$Id$
#pragma once
#include "Net.h"
#include "Dispatch.h"

namespace mones {

class IPStack
{
public:
    IPStack();
    virtual ~IPStack();
    void messageLoop();
    bool isStarted() {return started;}
    dword getThreadID() const { return this->myID; }
private:
    Dispatch* pDP;
    void getfreeport(MessageInfo* msg);
    void open(MessageInfo* msg);
    void read(MessageInfo* msg);
    void write(MessageInfo* msg);
    void close(MessageInfo* msg);
    void status(MessageInfo* msg);
    void config(MessageInfo* msg);
    word next_port;
protected:
    dword myID;
    dword timerid;
    bool started;
    bool loopExit;
};
}; // namespace mones
