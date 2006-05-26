#include "Nic.h"
using namespace mones;

ARPmanager::ARPmanager():ipaddress(0),netmask(0),defaultroute(0),registerd(0)
{
    for(int i=0;i<CACHESIZE;i++)
        memset(cache,'0',sizeof(ARPRec)*CACHESIZE);
}

Ether* ARPmanager::Query(dword dstip)
{
    Ether* frame = new Ether();   
    memset(frame->dstmac,0xFF,6);
    memcpy(frame->srcmac,macaddress,6);
    frame->type=bswap(TYPEARP);
    ARP* ah=frame->ARPHeader;
    ah->hardType=bswap(ARP::HARD_TYPE_ETHER);
    ah->protType=bswap(ARP::PROTCOL_TYPE_IP);
    ah->hardAddrLen=0x06;
    ah->protAddrLen=0x04;
    ah->opeCode=bswap(ARP::OPE_CODE_ARP_REQ);// word  opeCode;
    memcpy(ah->srcMac,macaddress,6); //byte  srcMac[6];
    ah->dstIp=dstip;                 // dword srcIp;
    memset(ah->dstMac,0x00,6);       // byte  dstMac[6];
    ah->srcIp=ipaddress;            //dword dstIp;
    return frame;
}

int ARPmanager::Lookup(byte* dstmac,dword dstip)
{
    for(int i=0;i<CACHESIZE;i++){
        if( dstip==cache[i].ip ){
            memcpy(dstmac,cache[i].mac,6);
            return 0;
        }
    }
    return -1;
}
int ARPmanager::Register(Ether* frame)
{
    ARP* ah=frame->ARPHeader;
    cache[registerd].ip=ah->srcIp;
    memcpy(cache[registerd].mac,ah->srcMac,6);
    registerd++;
    return 0;
}

int ARPmanager::MakeArpReply(Ether* frame)
{     
    ARP* ah = frame->ARPHeader;
    if( ah->opeCode == bswap(ARP::OPE_CODE_ARP_REP)){
        Register(frame);
        delete frame;
        return -1;
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
            return 0;
        }else{
            delete frame;
            return -1;
        }
    }else{
        printf("orz\n");
    }
    return -1;
}

word ARPmanager::CalcFrameSize(Ether* frame)
{
    if( frame->type==bswap(TYPEARP)){
        return 14+sizeof(ARP);
    }
    return 14+bswap(frame->IPHeader->len);
}

ARPmanager::~ARPmanager()
{

}

/////////////////////////////////////////////
Nic::Nic():irq(0),iobase(0),mtu(DEFAULT_MTU_SIZE)
{

}

Ether* Nic::Recv(int n)
{
    if( rxFrameList.size() > n  && n >=0 ){    
        Ether* frame = rxFrameList.removeAt(n);
        if( bswap(frame->type) ==  TYPEARP ){
            if( MakeArpReply(frame) == 0){
                Send(frame);
            }
            return NULL;
        }
        return frame;
    }
    return NULL;
}

Ether* Nic::MakePKT(dword dstip)
{
    Ether* frame= new Ether();
    memcpy(frame->srcmac,macaddress,6);    
    frame->type=bswap(TYPEIP);
    frame->IPHeader->srcip=ipaddress; 
    frame->IPHeader->dstip=dstip;
    for(int i=0;i<10;i++){
        if( Lookup(frame->dstmac,dstip) ==-1 ){
            Send(Query(dstip));
            sleep(300);
            interrupt();//copy rxdata form DMA memory. 
            Ether* f=Recv(0);
            if( f != NULL){
                rxFrameList.add(f);
            }
        }else{
            return frame;
        }
    }
    delete frame;
    return NULL;
}
