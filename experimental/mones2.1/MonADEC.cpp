//$Id$
#include "MonADEC.h"
using namespace MonAPI;
using namespace mones;

MonADEC::~MonADEC()
{

}

MonADEC::MonADEC()
{
 
}

int MonADEC::init()
{
    printf("Hello Virtual PC\n");
    return 0;
}

int MonADEC::interrupt()
{
    return 0;
}

void MonADEC::SendFrm(Ether* frame)
{

}
