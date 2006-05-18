#include "Nic.h"
using namespace mones;

ARPhandler::ARPhandler():ipaddress(0),netmask(0),defaultroute(0),head(0)
{
    for(int i=0;i<CACHESIZE;i++)
        memset(cache,'0',sizeof(ARPRec)*CACHESIZE);
}

int ARPhandler::Lookup(byte* dstmac,dword dstip)
{
    //search from head foward, if matched, then swap it with [head-1]
    //else Query and replace [head-1].
    for(int i=0;i<CACHESIZE;i++){
        int j= (head+i)&CACHESIZE;
        if( dstip==cache[j].ip ){
            //byte tmp[6];
            //TODO swap records.
            //backward shift the head.
            head--;
            if(head<0)
                head+=CACHESIZE;
            return 0;
        }
    }
    Query(dstip);
    sleep(3000);
    return -1;
}

int ARPhandler::SetHeader(Ether* frame)
{
    if( Lookup(macaddress,frame->IPHeader->dstip) == -1){
        if( Lookup(macaddress,frame->IPHeader->dstip) == -1){
            return -1;
        }
    }
    memcpy(frame->srcmac,macaddress,6);
    return 0;
}

void ARPhandler::Query(dword dstip)
{
    printf("Q\n");
    Ether* frame = new Ether();
    ARP* ah=frame->ARPHeader;
    ah->opeCode=bswap(ARP::OPE_CODE_ARP_REP);
    memset(ah->dstMac,'1',6);
    memcpy(ah->srcMac,macaddress,6); 
    ah->dstIp=ah->srcIp;
    ah->srcIp=ipaddress;
    memcpy(frame->dstmac,ah->dstMac,6);
    memcpy(frame->srcmac,ah->srcMac,6);
    Send(frame);
}

void ARPhandler::ARPreply(Ether* frame)
{     
    ARP* ah = frame->ARPHeader;
    if( ah->opeCode == bswap(ARP::OPE_CODE_ARP_REP)){
        printf("REP!");
        delete frame;
        frame=NULL;
    }else if( ah->opeCode==bswap(ARP::OPE_CODE_ARP_REQ)){
        if( ah->dstIp == ipaddress){
            ah->opeCode=bswap(ARP::OPE_CODE_ARP_REP);
            memcpy(ah->dstMac,ah->srcMac,6);
            memcpy(ah->srcMac,macaddress,6); 
            ah->dstIp=ah->srcIp;
            ah->srcIp=ipaddress;
            memcpy(frame->dstmac,ah->dstMac,6);
            memcpy(frame->srcmac,ah->srcMac,6);
            printf("ARP REQ\n");
        }else{
            delete frame;
            frame=NULL;
        }
    }
}

ARPhandler::~ARPhandler()
{

}

Nic::Nic():irq(0),iobase(0)
{

}

Ether* Nic::Recv(int n)
{
    if( rxFrameList.size() > n  && n >=0 ){    
        Ether* frame = rxFrameList.removeAt(n);
        if( bswap(frame->type) ==  TYPEARP ){
             ARPreply(frame);
             if( frame !=NULL)
                 Send(frame);
             return NULL;
        }
        return frame;
    }else{
        return NULL;
    }
}
