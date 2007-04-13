//$Id$
#include <pci/Pci.h>
#include "Net.h"
#include "NE2000.h"
#include "MonAMDpcn.h"
#include "LoopBack.h"
#include "Dispatch.h"

using namespace mones;
using namespace MonAPI;

void L4Base::Dump()
{
    printf("\nRIP%d LP%d RP%d CID%d DSC%d TICK%d DISPO:%d MSG:%x\n",
        remoteip,localport,remoteport,clientid ,netdsc, disposedtick, disposed, msg.header);
}

void L4Base::Close()
{
    dispatcher->RemoveInfo(this,0);
}

uint16_t L4Base::checksum(uint8_t *data,uint16_t size)
{
    uint32_t sum=0;
    for(int i=0;i<=size-2;i+=2){
        sum+=bswap(*(uint16_t*)(data+i));
    }
    if(size%2==1){
        sum+=bswap(*(data+size-1)&0xFF);
    }
    return ~(((sum>>16)+sum)&0xFFFF);
}

void L4Base::CreateIPHeader(Ether* frame,uint16_t length,uint8_t protocol)
{
    IP* ip=frame->IPHeader;
    ip->verhead=0x45;       //version & headersize
    ip->tos=0x00;           //tos
    ip->len=bswap(length);  //totallength PING size windows 60 solaris 84
    ip->id=bswap(dispatcher->getPacketID());   //made from PID?
    ip->frag=bswap(0x0000); //flag
    ip->ttl=0x80;           //TTL
    ip->prot=protocol;      
    ip->chksum=0x0;         //for calculating checksum, it should be zero.  
    ip->chksum=bswap(checksum((uint8_t*)ip,(ip->verhead&0x0F)<<2));
}

void L4Base::Read(MessageInfo* m)
{
    memcpy(&msg,(uint8_t*)m,sizeof(MessageInfo));
}

bool L4Base::TimeoutCheck(uint32_t now)
{
    if( disposed==true && disposedtick  < now ){
        return true;
    }
    return false;
}

bool L4Base::Read_bottom_half(Ether* frame)
{
    uint8_t* data;
    if( msg.header==MSG_NET_READ ){
        monapi_cmemoryinfo* mi = monapi_cmemoryinfo_new();  
        if (mi != NULL){
            if( frame != NULL ){
                int size=Strip(frame, &data);
                if( size >0 ){
                    monapi_cmemoryinfo_create(mi,size, true);
                    if( mi != NULL ){
                        memcpy(mi->Data,data,mi->Size);    
                        Message::reply(&msg, mi->Handle, mi->Size);
                        memset(&msg,'\0',sizeof(MessageInfo));
                    }
                }
            }
            monapi_cmemoryinfo_delete(mi);
        }else{
            Message::reply(&msg);
        }
        return true;
    }
    return false;
}

void L4Base::Write(MessageInfo* m)
{ 
    monapi_cmemoryinfo* ret = monapi_cmemoryinfo_new();
    if( ret != NULL){
        ret->Handle = m->arg2;
        ret->Owner  = m->from;
        ret->Size   = m->arg3;
        monapi_cmemoryinfo_map(ret);
        if( netdsc == m->arg1 ){
            dispatcher->Send(ret->Data,ret->Size,this);           
            memcpy(&msg,(uint8_t*)m,sizeof(MessageInfo)); //Register msg.
        }
        monapi_cmemoryinfo_delete(ret);
    }
    Message::reply(m);
}
