//$Id: Dispatch.cpp 3263 2006-06-09 18:05:20Z eds1275 $
#include <pci/Pci.h>
#include "Net.h"
#include "NE2000.h"
#include "MonAMDpcn.h"
#include "LoopBack.h"
#include "Dispatch.h"

using namespace mones;
using namespace MonAPI;

word ConnectionInfo::checksum(byte *data,word size)
{
    dword sum=0;
    for(int i=0;i<=size-2;i+=2){
        sum+=bswap(*(word*)(data+i));
    }
    if(size%2==1){
        sum+=bswap(*(data+size-1)&0xFF);
    }
    return ~(((sum>>16)+sum)&0xFFFF);
}

void ConnectionInfo::CreateIPHeader(Ether* frame,word length,byte protocol)
{
    IP* ip=frame->IPHeader;
    ip->verhead=0x45;       //version & headersize
    ip->tos=0x00;           //tos
    ip->len=bswap(length);  //totallength PING size windows 60 solaris 84
    ip->id=bswap(0xabcd);   //made from PID?
    ip->frag=bswap(0x0000); //flag
    ip->ttl=0x80;           //TTL
    ip->prot=protocol;      
    ip->chksum=0x0;         //for calculating checksum, it should be zero.  
    ip->chksum=bswap(checksum((byte*)ip,(ip->verhead&0x0F)<<2));
}
////////////////////////////////////////////////
void ICMPCoInfo::CreateHeader(Ether* frame,byte* data, word size)
{
    ICMP* icmp=frame->IPHeader->ICMPHeader;
    icmp->type=type;
    icmp->code=0x00;
    icmp->chksum=0x0000;
    icmp->idnum=bswap(idnum);
    icmp->seqnum=bswap(seqnum);    
    memcpy(icmp->data,data,size);
    icmp->chksum=bswap(checksum((byte*)icmp,size+sizeof(ICMP)));
    CreateIPHeader(frame,size+sizeof(ICMP)+sizeof(IP),TYPEICMP);
}

int ICMPCoInfo::Strip(Ether* frame, byte** data)
{
   *data=frame->IPHeader->ICMPHeader->data;
   return bswap(frame->IPHeader->len)-sizeof(IP)-sizeof(ICMP);
}

bool ICMPCoInfo::IsMyPacket(Ether* frame)
{
    if( WellKnownSVCreply(frame) ){
        return true;
    }else if ( TYPEICMP  == frame->IPHeader->prot ){
        ICMP* icmp=frame->IPHeader->ICMPHeader;
         if( ECHOREPLY == icmp->type  &&
            remoteip  == frame->IPHeader->srcip &&
            idnum     == bswap(icmp->idnum) &&
            seqnum    == bswap(icmp->seqnum) )
        {
            return true;
        }
    }
    return false;
}

bool ICMPCoInfo::WellKnownSVCreply(Ether* frame)
{
    if( TYPEICMP == frame->IPHeader->prot ){
        ICMP* icmp=frame->IPHeader->ICMPHeader;
        if( ECHOREQUEST==icmp->type ){
            ICMPCoInfo info(dispatcher);
            info.remoteip=frame->IPHeader->srcip;
            info.type=ECHOREPLY;
            info.idnum=bswap(icmp->idnum);
            info.seqnum=bswap(icmp->seqnum);
            dispatcher->Send(icmp->data,bswap(frame->IPHeader->len)-sizeof(IP)-sizeof(ICMP),&info);
            return true;
        }
    }
    return false;
}
/////////////////////////////////////////////////
void UDPCoInfo::CreateHeader(Ether* frame,byte* data, word size)
{
    IP* ip=frame->IPHeader; //for psedo header
    ip->ttl =0x00;
    ip->prot=TYPEUDP;
    ip->chksum=bswap(size+sizeof(UDP));
    UDP* udp=frame->IPHeader->UDPHeader;
    udp->srcport=bswap(localport);
    udp->dstport=bswap(remoteport);
    udp->len=bswap(size+sizeof(UDP));
    udp->chksum=0x0000;
    memcpy(frame->IPHeader->UDPHeader->data,data,size);
    udp->chksum=bswap(checksum((byte*)&(ip->ttl),size+sizeof(UDP)+12));
    CreateIPHeader(frame,size+sizeof(UDP)+sizeof(IP),TYPEUDP);
}

int UDPCoInfo::Strip(Ether* frame, byte** data)
{   
    *data=frame->IPHeader->UDPHeader->data;
    return bswap(frame->IPHeader->len)-sizeof(IP)-sizeof(UDP);
}

bool UDPCoInfo::IsMyPacket(Ether* frame)
{
    if( WellKnownSVCreply(frame) ){
        return true;
    }else if( TYPEUDP    == frame->IPHeader->prot &&
              remoteip   == frame->IPHeader->srcip &&
              localport  == bswap(frame->IPHeader->UDPHeader->dstport) &&
              remoteport == bswap(frame->IPHeader->UDPHeader->srcport)
            ){
        return true;
    }
    return false;
}

bool UDPCoInfo::WellKnownSVCreply(Ether* frame)
{   
    if( TYPEUDP == frame->IPHeader->prot ){
        if( frame->IPHeader->UDPHeader->dstport==bswap(DAYTIME)){
            UDPCoInfo cinfo(dispatcher);
            cinfo.remoteip=frame->IPHeader->srcip;
            cinfo.localport=DAYTIME;
            cinfo.remoteport=bswap(frame->IPHeader->UDPHeader->srcport);
            char* data="I can't see a clock.";    
            dispatcher->Send((byte*)data,20,&cinfo);
            printf("T\n");
            return true;
        }
    }
    return false;
}

///////////////////////////////////////////////////

void TCPCoInfo::CreateHeader(Ether* frame,byte* data, word size)
{      
    HandShakeACTV(frame);
    CreateIPHeader(frame,size+sizeof(TCP)+sizeof(IP),TYPETCP);
}

int TCPCoInfo::Strip(Ether* frame, byte** data)
{
    *data=frame->IPHeader->TCPHeader->data;
    return bswap(frame->IPHeader->len)-sizeof(IP)-sizeof(TCP);
}

bool TCPCoInfo::IsMyPacket(Ether* frame)
{
    if( HandShakePASV(frame) ){
        return false;
    }else 
    if( WellKnownSVCreply(frame) ){
        return true;
    }else if( TYPETCP    == frame->IPHeader->prot &&
              remoteip   == frame->IPHeader->srcip &&
              localport  == bswap(frame->IPHeader->TCPHeader->dstport) &&
              remoteport == bswap(frame->IPHeader->TCPHeader->srcport)
            ){
        return true;
    }
    return false;
}

bool TCPCoInfo::WellKnownSVCreply(Ether* frame)
{    
    if( TYPETCP == frame->IPHeader->prot ){
        if( frame->IPHeader->UDPHeader->dstport==bswap(DAYTIME)){
            UDPCoInfo cinfo(dispatcher);
            cinfo.remoteip=frame->IPHeader->srcip;
            cinfo.localport=DAYTIME;
            cinfo.remoteport=bswap(frame->IPHeader->UDPHeader->srcport);
            char* data="I can't see a clock.";    
            dispatcher->Send((byte*)data,20,&cinfo);
            return true;
        }
    }
    return false;
}

//   CLOSED       -> LISTENING @readAPI    ==
//   ESTABLISHED  -> FIN_WAIT1 @closeAPI   send FIN
//   ~ESTABLISHED -> CLOSED    @timerAPI   ==
bool TCPCoInfo::HandShakeACTV(Ether* frame)
{
    printf("ACTV\n");
    //CLOSED     -> SYN_SENT               send SYN
    //SYN_RCVD   -> SYN_RCVD               send SYN+ACK
    //SYN_SENT   -> ESTABLISHED            send ACK
    //CLOSE_WAIT -> LAST_ACK               send FIN
    //TIME_WAIT  -> TIME_WAIT              send ACK
    //ESTABLISHED->ESTABLISHED             send -
    return false;
}

bool TCPCoInfo::HandShakePASV(Ether* frame)
{
    printf("PASV\n");
    //CLOSED     -> CLOSED                 ==
    //LISTENING  -> SYN_RCVD               rcv SYN  (send SYN+ACK)
    //SYN_SENT   -> SYN_SENT               rcv SYN+ACK (send ACK)
    //SYN_RCVD   -> ESTABLISHED            rcv ACK
    //ESTABLISHED-> CLOSE_WAIT             rcv FIN
    //FIN_WAIT2  -> TIME_WAIT              rcv ACK
    //LAST_ACK   -> CLOSED                 rcv ACK
    //ESTABLISHED-> ESTABLISHED            rcv -
    return false;
}
