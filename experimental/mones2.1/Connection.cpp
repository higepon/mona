//$Id$
#include <pci/Pci.h>
#include "Net.h"
#include "NE2000.h"
#include "MonAMDpcn.h"
#include "LoopBack.h"
#include "Dispatch.h"

using namespace mones;
using namespace MonAPI;
void ConnectionInfo::Close()
{
    dispatcher->RemoveConnection(this,0);
}

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
    if( TYPEICMP  == frame->IPHeader->prot ){
        if( WellKnownSVCreply(frame) ){
            return true;
        }
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
    ICMP* icmp=frame->IPHeader->ICMPHeader;
    if( ECHOREQUEST==icmp->type ){
        remoteip=frame->IPHeader->srcip;
        type=ECHOREPLY;
        idnum=bswap(icmp->idnum);
        seqnum=bswap(icmp->seqnum);
        dispatcher->Send(icmp->data,bswap(frame->IPHeader->len)-sizeof(IP)-sizeof(ICMP),this);
        return true;
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
    /////////
    UDP* udp=frame->IPHeader->UDPHeader;
    udp->srcport=bswap(localport);
    udp->dstport=bswap(remoteport);
    udp->len=bswap(size+sizeof(UDP));
    udp->chksum=0x0000;
    memcpy(udp->data,data,size);
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
    if( TYPEUDP    == frame->IPHeader->prot ){
        if( WellKnownSVCreply(frame) ){
            return true;
        }
        if( remoteip   == frame->IPHeader->srcip &&
            localport  == bswap(frame->IPHeader->UDPHeader->dstport) &&
            remoteport == bswap(frame->IPHeader->UDPHeader->srcport) )
        {
            return true;
        }
    }
    return false;
}

bool UDPCoInfo::WellKnownSVCreply(Ether* frame)
{   
    if( frame->IPHeader->UDPHeader->dstport==bswap(DAYTIME)){
        remoteip=frame->IPHeader->srcip;
        localport=DAYTIME;
        remoteport=bswap(frame->IPHeader->UDPHeader->srcport);
        char* data="I can't see a clock.";    
        dispatcher->Send((byte*)data,20,this);
        return true;
    }
    return false;
}

///////////////////////////////////////////////////

int TCPCoInfo::Strip(Ether* frame, byte** data)
{
    *data=frame->IPHeader->TCPHeader->data;
    return bswap(frame->IPHeader->len)-sizeof(IP)-sizeof(TCP);
}

bool TCPCoInfo::IsMyPacket(Ether* frame)
{
    if( TYPETCP    == frame->IPHeader->prot){ //YES! MYPACKET
        if( TransStateByPKT(frame) ){
            if( WellKnownSVCreply(frame) ){
                return true;
            }else if( remoteip   == frame->IPHeader->srcip &&
                  localport  == bswap(frame->IPHeader->TCPHeader->dstport) &&
                  remoteport == bswap(frame->IPHeader->TCPHeader->srcport) )
            {
                return true;
            }
        }
    }
    return false;
}

bool TCPCoInfo::WellKnownSVCreply(Ether* frame)
{    
    if( frame->IPHeader->UDPHeader->dstport==bswap(DAYTIME)){
        remoteip=frame->IPHeader->srcip;
        localport=DAYTIME;
        remoteport=bswap(frame->IPHeader->UDPHeader->srcport);
        char* data="I can't see a clock.";    
        dispatcher->Send((byte*)data,20,this);
        return true;
    }
    return false;
}

void TCPCoInfo::CreateHeader(Ether* frame,byte* data, word size)
{
    IP* ip=frame->IPHeader; //for psedo header
    ip->ttl =0x00;
    ip->prot=TYPETCP;
    ip->chksum=bswap(size+sizeof(TCP));
    /////////
    TCP* tcp=frame->IPHeader->TCPHeader;
    tcp->srcport=bswap(localport);
    tcp->dstport=bswap(remoteport);
    tcp->seqnumber=bswapl(seqnum);
    tcp->acknumber=bswapl(acknum);
    tcp->offset=0xF0 & ((sizeof(TCP))<<2);//options.
    tcp->flags=0x3F&flags;   
    tcp->window=bswap(window);
    tcp->chksum=0x0000;
    tcp->urgent=0;
    memcpy(tcp->data,data,size);
    tcp->chksum=bswap(checksum((byte*)&(ip->ttl),size+sizeof(TCP)+12));
    CreateIPHeader(frame,size+sizeof(TCP)+sizeof(IP),TYPETCP);
    //printf("CreateHeader\n");
}


void TCPCoInfo::Close()
{     
    if( status == ESTAB ){
        //seqnum;
        //acknum;
        status=FIN_WAIT1;
        flags=FIN|ACK;
        window=1500; 
        printf("ESTAB->FIN_WAIT1\n");
        dispatcher->Send(NULL,0,this);
    }
}

/*      CLOSED=1,
        LISTENING,
        SYN_SENT,
        SYN_RCVD,
        ESTAB,
        FIN_WAIT1,
        FIN_WAIT2,
        CLOSE_WAIT,
        LAST_ACK,
        TIME_WAIT */
//=EVENT==========STAT.TRANS.============ACTION=================WHO
  //MSGACTVOpen    CLOSED->SYN_SENT       sendSYN               @THIS
//MSGOpen         CLOISED->LISTEN        -
//MSGWrite        LISTEN->ERROR.
//MSGClose        SYN_RCVD->FIN_WAIT1    sendFIN
//MSGClose        ESTAB->FIN_WAIT1       sendFIN
  //recvSYN|ACK     SYN_SENT-> ESTAB       sendACK replyMSGOPEN @THIS
//recvSYN         SYN_SENT->SYN_RCVD     sendACK
//recvSYN         LISTEN->SYN_RCVD       sendSYN|ACK
//recvACK         SYN_RSVD->ESTAB        -
//recvFIN         ESTAB->CLOSE_WAIT      sendACK
//TRANS.CLOS_WAIT CLOSE_WAIT->LAST_ACK   sendFIN
//recvFIN|ACK     LAS_ACK->CLOSED
//recvFIN|ACK     FIN_WAIT1->FIN_WAIT2   -
//recvFIN         FIN_WAIT2->TIME_WAIT   sendACK
//recvACK         FIN_WAIT1->CLOSING     sendACK
//recvFIN|ACK     CLOSEING->TIME_WAIT    TRANS.CLOSED delay2MIN.
//MSGRead         ESTAB->ESTAB           -
//MSGWrite        ESTAB->ESTAB           -
//MSGRead         OTHERS->OTHRES         replyERROR
//MSGWrite        OTHERS->OTHERS         replyERROR
//MSGClose        OTHERS->CLOSED         -

bool TCPCoInfo::TransStateByMSG(dword msg)
{
    if( status == CLOSED  && isPasv==false && msg==MSG_NET_OPEN){
        seqnum=bswapl(1);
        acknum=bswapl(1);
        status=SYN_SENT;
        flags=SYN;    
        window=1500;
        printf("CLOSED->SYN_SENT\n");
        dispatcher->Send(NULL,0,this);
        return false;
    }
    if( status == ESTAB ){
        flags=NORM;
        printf("XX");
    }

    return false; //Not Dispose Packet.
    //return true //Dispose Packet.
}

bool TCPCoInfo::TransStateByPKT(Ether* frame)
{

    byte rflag= frame->IPHeader->TCPHeader->flags;
    //printf("%x %d\n",rflag,status);
    if( (status == SYN_SENT) && (rflag == SYN|ACK) ){
        printf("<<%d>>\n",bswap(frame->IPHeader->id));
        remoteip=frame->IPHeader->srcip;
        seqnum=bswapl(frame->IPHeader->TCPHeader->acknumber);
        acknum=bswapl(frame->IPHeader->TCPHeader->seqnumber)+1;
        status=ESTAB; 
        flags=ACK;
        window=1502;
        printf("SYN_SENT->ESTAB");
        Message::reply(&msg, netdsc);
        dispatcher->Send(NULL,0,this);
        return false;
    }
    if( (status == FIN_WAIT1) && (rflag == (FIN|ACK)) ){
        remoteip=frame->IPHeader->srcip;
        seqnum=bswapl(frame->IPHeader->TCPHeader->acknumber);
        acknum=bswapl(frame->IPHeader->TCPHeader->seqnumber)+1;
        status=TIME_WAIT; 
        flags=ACK;
        window=1501;
        printf("FIN_WAIT1->TIME_WAIT %d %d\n",status,rflag);
        Message::reply(&msg, netdsc);//close
        dispatcher->Send(NULL,0,this);
        dispatcher->RemoveConnection(this,500);
        return false;
    }
    return true; //Dispose Packet.
    //return false; //Not Dispose Packet.
}

void TCPCoInfo::SendACK(Ether* frame)
{
     byte* tmp;
     int size=Strip(frame,&tmp);
     if( size == 0 ) size =1 ;
     remoteip=frame->IPHeader->srcip;
     seqnum=bswapl(frame->IPHeader->TCPHeader->acknumber);
     acknum=bswapl(frame->IPHeader->TCPHeader->seqnumber)+size;
     status=ESTAB; 
     flags=ACK;    
     window=1500;
     printf("SENDING READ ACK");
     dispatcher->Send(NULL,0,this);
}
