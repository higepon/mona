//$Id$ 
//Acknowledgement:
//   Following files were originally written by Yamami and Higepon.
//   Makefile,Nic.h,NE2000.cpp,NE2000.h
#include "IPStack.h"
#include <monapi.h>
#include <monalibc.h>
#include <monalibc/stdio.h>

using namespace mones;
using namespace MonAPI;

int MonaMain(List<char*>* pekoe)
{
    IPStack* ipstack=new IPStack();
    ipstack->messageLoop();
    delete ipstack;
    return 0;
}
