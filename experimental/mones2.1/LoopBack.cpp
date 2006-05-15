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
    word ret=0;
    return ret;
}

void LoopBack::Send(Ether::Frame* frame)
{                  
    //What should i do?     
}
