#include "NetClient.h"
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

int NetClient::Open(dword remoteip, word localport, word remoteport, word protocol)
{
    MessageInfo msg;
    dword port=((localport)<<16)|remoteport;
    if (Message::sendReceive(&msg, serverid, MSG_NET_OPEN, remoteip,port,protocol,NULL) != 0){
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
    if (Message::sendReceive(&msg, serverid, MSG_NET_READ,(dword)noblock) != 0){
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
            Message::sendReceive(&msg, serverid, MSG_NET_WRITE,ret->Handle,ret->Size);
        }
        monapi_cmemoryinfo_delete(ret);
    }
    return msg.arg2;
}

int NetClient::Stat(char* if_name,NetStatus* stat)
{
    monapi_cmemoryinfo* ret;
    MessageInfo msg;
    if (Message::sendReceive(&msg, serverid, MSG_NET_STATUS,0,0,0,if_name) != 0){
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

int NetClient::Example()
{
    char devname[]="pcnet0";
    sleep(1000);
    NetStatus stat;
    if( !Stat(devname,&stat) ){
        printf("StatError.\n");
    }  
    printf("[%d]\n",stat.a);

    dword remoteip=(1<<24)|(177<<16)|(16<<8)|(172);
    word localport = GetFreePort();
    printf("Port=%d\n",localport);
    //UNIX inetd has DAYTIME,ECHO services in both UDP and TCP.
    int netdsc = Open(remoteip,localport,DAYTIME,TYPEUDP);
    if( netdsc < 0 ){
        printf("OpenError.\n");
    } 
    printf("netdsc=%d\n",netdsc);

    if( Write(netdsc,(byte*)"test",4) ){
        printf("WrieError.\n");
    }
    byte buf[1024];//BAD design.
    int size= Read(netdsc,buf);
    if( size > 0 ){
        printf("Read: %s\n",buf);
    }    

    if( Close(netdsc) ){
        printf("CloseError.\n");
    }    

    if( Config(devname,(5<24)|(177<16)|(16<<8)|172,(1<24)|(177<16)|(16<<8)|172,24,60,1500) ){
        printf("ConfigError\n");
    }
    printf("TESTED\n");
    return 0;
}

int NetClient::initalize(dword threadid)
{
    serverid=threadid; 
    clientid=System::getThreadID();
    return 0;
}
