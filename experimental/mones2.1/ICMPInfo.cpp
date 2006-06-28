//$Id: Connection.cpp 3287 2006-06-18 07:17:51Z eds1275 $
#include "Dispatch.h"
#include "ICMPInfo.h"
using namespace mones;
using namespace MonAPI;

void ICMPCoInfo::Dump()
{
    L4Base::Dump();
    printf("SEQ:%d ID%d\n",seqnum,idnum);
}

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

bool ICMPCoInfo::IsProcessed(Ether* frame)
{
    Read_bottom_half(frame);
    return true;
}
bool ICMPCoInfo::IsMyPacket(Ether* frame)
{
    if( TYPEICMP  == frame->IPHeader->prot ){
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

bool ICMPCoInfo::Reply(Ether* frame)
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
