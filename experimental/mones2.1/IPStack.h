//$Id$
#pragma once
#include <Net.h>
#include <monapi.h>
#include <sys/types.h>
#include "Dispatch.h"
#include "TCPIO.h"

namespace mones {


//Timer management class for TCP connection.
class TimeKeeper{
public:
    TimeKeeper();
     ~TimeKeeper();
     void init_DelayAck();
     void init_ReTransmit();
     void init_KeepAlive();
     void init_SegmentLifeTime();
     void init_Probe();
     void init_Write();
     void init_Read();
     static void ThreadMain(void*);
private:
     dword timerid;
     int delayack;
     int retransmit;
     int keepalive;
     int Segmentlifetime;
     int probe;
     int write;
     int read;
};
//////////////////////////////////////////////
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
    void icmpopen(MessageInfo* msg);
    void udpopen(MessageInfo* msg);
    void tcpactvopen(MessageInfo* msg);
    void tcppasvopen(MessageInfo* msg);
    void tcpaccept(MessageInfo* msg);
    void read(MessageInfo* msg);
    void write(MessageInfo* msg);
    void close(MessageInfo* msg);
    void getstatus(MessageInfo* msg);
    void config(MessageInfo* msg);
    void reset(MessageInfo* msg);
    void setblockingmode(MessageInfo* msg);
    word next_port;   
    TimeKeeper tcp_timer;
    TCPIn tcp_in;
    TCPOut tcp_out;
protected:
    dword myID;
    bool started;
    bool loopExit;
};
}; // namespace mones
