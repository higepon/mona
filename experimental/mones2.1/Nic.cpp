#include "Nic.h"
using namespace mones;

//////////////
Nic::Nic()
{   
    myIP=0;
    irq=0;
    iobase=0;
}

Nic::~Nic()
{
}

Ether::Frame* Nic::Recv(int n)
{
	if( rxFrameList.size() > n  && n >=0 ){
		return rxFrameList.removeAt(n);
	}else{
		return NULL;
	}
}
