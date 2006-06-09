//$Id$
#pragma once
#include "Net.h"
#include "Dispatch.h"

namespace mones {

class NetServer: public Dispatch
{
public:
    NetServer();
    virtual ~NetServer();
    dword getThreadID() const;
    void messageLoop();
    bool isStarted() {return started;}
private:
    MonAPI::Mutex mutex;
    void getfreeport(MessageInfo* msg);
    void Interrupt(MessageInfo* msg);
    void open(MessageInfo* msg);
    void read(MessageInfo* msg);
    void write(MessageInfo* msg);
    void close(MessageInfo* msg);
    void status(MessageInfo* msg);
    void ontimer(MessageInfo* msg);
    void config(MessageInfo* msg);
    word next_port;
protected:
    dword myID;
    dword timerid;
    bool started;
    bool loopExit;
};
}; // namespace mones
