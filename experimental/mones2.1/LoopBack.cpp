#include "LoopBack.h"
using namespace MonAPI;
using namespace mones;

LoopBack::~LoopBack()
{

}

LoopBack::LoopBack()
{

}

int LoopBack::interrupt()
{
    return (TX_INT|RX_INT);
}

void LoopBack::Send(Ether::Frame* frame)
{                  
    rxFrameList.add(frame); //Not TX but RX!
    MessageInfo info;
    Message::create(&info, MSG_INTERRUPTED, 0, 0, 0, NULL);
    //What should i do? should i send a message to parent Thead?
    Message::send(System::getThreadID(), &info);
}
