//$Id$
#include <NetClient.h>
#include <monalibc/stdio.h>
using namespace MonAPI;
using namespace mones;

word NetClient::GetFreePort()
{
    MessageInfo msg;
    if(Message::sendReceive(&msg,serverid,MSG_NET_GETFREEPORT) !=0 ){
        return 0;
    }
    return msg.arg2;
}

int NetClient::Config(char* if_name, dword localip, dword gatewayip, byte subnetmask, byte timeout, word mtu)
{
    MessageInfo msg;
    dword mask_timeout_mtu=((subnetmask<<24)&0xFF0000)|((timeout<<16)&0x00FF0000)|(mtu&0x0000FFFF);
    if (Message::sendReceive(&msg,serverid,MSG_NET_CONFIG,localip,gatewayip,mask_timeout_mtu,if_name)!=0){
        return -1;
    }
    return msg.arg2;
}

int NetClient::ICMPOpen(dword remoteip)
{
    MessageInfo msg;
    if (Message::sendReceive(&msg, serverid, MSG_NET_ICMPOPEN, remoteip) != 0){
        return -1;
    }
    return msg.arg2;

}

int NetClient::UDPOpen(dword remoteip, word localport, word remoteport)
{
    MessageInfo msg;
    dword port=((localport)<<16)|remoteport;
    if (Message::sendReceive(&msg, serverid, MSG_NET_UDPOPEN, remoteip,port) != 0){
        return -1;
    }
    return msg.arg2;

}

int NetClient::TCPActvOpen(dword remoteip, word localport, word remoteport)
{
    MessageInfo msg;
    dword port=((localport)<<16)|remoteport;
    if (Message::sendReceive(&msg, serverid, MSG_NET_ACTVOPEN, remoteip,port) != 0){
        return -1;
    }
    return msg.arg2;
}

int NetClient::TCPPasvOpen( word remoteport )
{
    MessageInfo msg;
    if (Message::sendReceive(&msg, serverid, MSG_NET_PASVOPEN,remoteport) != 0){
        return -1;
    }
    return msg.arg2;
}

int NetClient::TCPAccept(int netdsc)
{
    MessageInfo msg;
    if (Message::sendReceive(&msg, serverid, MSG_NET_ACCEPT, netdsc) != 0){
        return -1;
    }
    return msg.arg2;
}

int NetClient::Close(int netdsc)
{  
    MessageInfo msg;
    if (Message::sendReceive(&msg, serverid, MSG_NET_CLOSE, netdsc) != 0){
        return MONA_FAILURE;
    }
    return msg.arg2;
}

int NetClient::Read(int netdsc,byte* data,bool noblock )
{   
    monapi_cmemoryinfo* ret;
    MessageInfo msg;
    if (Message::sendReceive(&msg, serverid, MSG_NET_READ,(dword)netdsc,(dword)noblock) != 0){
        return NULL;
    }
    if (msg.arg2 == 0) return NULL;
    ret = monapi_cmemoryinfo_new();
    ret->Handle = msg.arg2;
    ret->Owner  = serverid;
    ret->Size   = msg.arg3;
    monapi_cmemoryinfo_map(ret);
    memcpy(data,ret->Data,ret->Size);data[ret->Size]='\0';
    int size=ret->Size;
    monapi_cmemoryinfo_delete(ret);
    return size;
}

int NetClient::Write(int netdsc,byte* data,word size)
{  
    MessageInfo msg;
    monapi_cmemoryinfo* ret = monapi_cmemoryinfo_new();  
    if (ret != NULL){    
        monapi_cmemoryinfo_create(ret, size, true);        
        if( ret != NULL ){
            memcpy(ret->Data,data,size);
            Message::sendReceive(&msg, serverid, MSG_NET_WRITE,netdsc,ret->Handle,ret->Size);
        }
        monapi_cmemoryinfo_delete(ret);
    }
    return msg.arg2;
}

int NetClient::Stat(NetStatus* stat)
{
    monapi_cmemoryinfo* ret;
    MessageInfo msg;
    if (Message::sendReceive(&msg, serverid, MSG_NET_GETSTATUS) != 0){
        return NULL;
    }
    if (msg.arg2 == 0) return NULL;
    ret = monapi_cmemoryinfo_new();
    ret->Handle = msg.arg2;
    ret->Owner  = serverid;
    ret->Size   = msg.arg3;
    monapi_cmemoryinfo_map(ret);
    memcpy(stat,ret->Data,ret->Size);
    int size=ret->Size;
    monapi_cmemoryinfo_delete(ret);
    return size;
}

int NetClient::Reset(dword remoteip,word localport, word remoteport)
{
    MessageInfo msg;
    dword port=((localport)<<16)|remoteport;
    if (Message::sendReceive(&msg, serverid, MSG_NET_RESET, remoteip,port) != 0){
        return -1;
    }
    return msg.arg2;
}

int NetClient::SetBlockingMode(int netdsc,int mode )
{
    MessageInfo msg;
    if (Message::sendReceive(&msg, serverid, MSG_NET_SETBLKMODE, netdsc, mode) != 0){
        return -1;
    }
    return msg.arg2;
}

NetClient::NetClient()
{
    serverid=Message::lookupMainThread("IPSTACK.EX5");
    clientid=System::getThreadID();
    //printf("ServerID %d ClientID %d\n",serverid,clientid);
    //TODO error check.
}
