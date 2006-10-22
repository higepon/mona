#include "TimeKeeper.h"

void TimeKeeper::ThreadMain(void*)
{
    for(;;){
        printf(">>\n");
        sleep(1000);
    }
    return;
}

TimeKeeper::TimeKeeper():timerid(0),delayack(0),
    retransmit(0),keepalive(0),Segmentlifetime(0),probe(0),write(0),read(0)
{    
    timerid=set_timer(500); 
}    

TimeKeeper::~TimeKeeper()
{ 
    if( timerid != 0)
        kill_timer(timerid);
}

void TimeKeeper::init_DelayAck()
{

}

void TimeKeeper::init_ReTransmit()
{

}
    
void TimeKeeper::init_KeepAlive()
{
}
    
void TimeKeeper::init_SegmentLifeTime()
{

}

void TimeKeeper::init_Probe()
{

}

void TimeKeeper::init_Write()
{

}

void TimeKeeper::init_Read()
{

}
