//$Id$
#pragma once
#include <monapi.h>
#include <sys/types.h>

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

