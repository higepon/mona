//$Id$
#include "LoopBack.h"
using namespace MonAPI;
using namespace mones;

LoopBack::LoopBack()
{
    memcpy(devname,"loopback",9);
}

LoopBack::~LoopBack()
{

}

int LoopBack::interrupt()
{
    return (TX_INT|RX_INT);
}

void LoopBack::Send(Ether* frame)
{                  
    rxFrameList.add(frame); //Not TX but RX!
    MessageInfo info;
    Message::create(&info, MSG_INTERRUPTED, 0, 0, 0, NULL);
    Message::send(System::getThreadID(), &info);
}
