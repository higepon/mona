#include "Nic.h"
using namespace mones;

int ARPhandler::SetHeader(byte* dst, IP* IPHeader)
{
    byte src[6];
    memcpy(dst,src,6);
    memcpy(dst+6,src,6);
    memcpy(dst+12,src,2);
    printf("Fill\n");
    return 0;
}

void ARPhandler::ARPreply(Ether* frame)
{    
    //handling ARP without server.
    printf("ARP");
    ARP* ah = frame->ARPHeader;
    if( ah->dstIp == ipaddress){
        ah->opeCode=bswap(ARP::OPE_CODE_ARP_REP);
        memcpy(ah->dstMac,ah->srcMac,6);
        getMacAddress(ah->srcMac); 
        ah->dstIp=ah->srcIp;
        ah->srcIp=ipaddress;
        memcpy(frame->dstmac,ah->dstMac,6);
        memcpy(frame->srcmac,ah->srcMac,6);
    }else{
        delete frame;
    }    
}

ARPhandler::~ARPhandler()
{

}

//////////////
Nic::Nic()
{    
    //todo initalize
    irq=0;
    iobase=0;
    ipaddress=0;
    netmask=0;
    defaultroute=0;
}


Ether* Nic::Recv(int n)
{           
    if( rxFrameList.size() > n  && n >=0 ){    
        Ether* frame = rxFrameList.removeAt(n);
        if( bswap(frame->type) ==  Ether::TYPEARP ){
             ARPreply(frame);
             Send(frame);
             return NULL;
        }
        return frame;
    }else{
        return NULL;
    }
}
