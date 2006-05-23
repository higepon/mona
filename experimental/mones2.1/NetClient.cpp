#include "NetClient.h"
using namespace MonAPI;
using namespace mones;

/*typedef struct {
    dword header;
    dword arg1;
    dword arg2;
    dword arg3;
    dword from;
    char str[128];
    int length;
} MessageInfo; */

word NetClient::GetFreePort()
{
    MessageInfo msg;
    if(Message::sendReceive(&msg,serverid,MSG_NET_GETFREEPORT) !=0 ){
        return 0;
    }
    return msg.arg2;
}

int NetClient::Config(dword localip, dword gatewayip, word subnetmask, word mtu)
{
    MessageInfo msg;
    dword mask_mtu=(subnetmask<<16)|mtu;
    if (Message::sendReceive(&msg, serverid, MSG_NET_CONFIG,localip,gatewayip,mask_mtu,NULL) != 0){
        return -1;
    }
    return 0;
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

int NetClient::Close()
{  
    MessageInfo msg;
    dword handle=0;
    if (Message::sendReceive(&msg, serverid, MSG_NET_CLOSE, handle) != 0){
        return MONA_FAILURE;
    }
    return msg.arg2;
}

monapi_cmemoryinfo* NetClient::Read() //data
{   
    monapi_cmemoryinfo* ret;
    MessageInfo msg;
    if (Message::sendReceive(&msg, serverid, MSG_NET_READ, 0, 0, 0, NULL) != 0){
        return NULL;
    }
    if (msg.arg2 == 0) return NULL;
    ret = monapi_cmemoryinfo_new();
    ret->Handle = msg.arg2;
    ret->Owner  = serverid;
    ret->Size   = msg.arg3;
    monapi_cmemoryinfo_map(ret);
    return ret;
}

int NetClient::Write() //data,size,
{
    monapi_cmemoryinfo* ret;
    MessageInfo msg;
    if (Message::sendReceive(&msg, serverid, MSG_NET_READ, 0, 0, 0, NULL) != 0){
        return NULL;
    }
    if (msg.arg2 == 0) return NULL;
    ret = monapi_cmemoryinfo_new();
    ret->Handle = msg.arg2;
    ret->Owner  = serverid;
    ret->Size   = msg.arg3;
    monapi_cmemoryinfo_map(ret);
    return 0;
}

monapi_cmemoryinfo*  NetClient::Stat()//data
{
    monapi_cmemoryinfo* ret;
    MessageInfo msg;
    if (Message::sendReceive(&msg, serverid, MSG_NET_READ, 0, 0, 0, NULL) != 0){
        return NULL;
    }
    if (msg.arg2 == 0) return NULL;
    ret = monapi_cmemoryinfo_new();
    ret->Handle = msg.arg2;
    ret->Owner  = serverid;
    ret->Size   = msg.arg3;
    monapi_cmemoryinfo_map(ret);
    return ret;
}

int NetClient::Test()
{
    sleep(1000);
    dword remoteip=(1<<24)|(177<<16)|(16<<8)|(172);
    word port = GetFreePort();
    if( port == 0 )
        printf("Error GetFreePort\n");
    int netdsc = Open(remoteip,port,DAYTIME,TYPETCP);
    if( netdsc < 0 ){
        printf("OpenError.\n");
    } 
    printf("netdsc=%d\n",netdsc);
    if( Write() ){
        printf("WrieError.\n");
    }
    if( Read( ) ){
        printf("ReadError.\n");
    }
    if( Close() ){
        printf("CloseError.\n");
    }
    if( Stat() ){
        printf("StatError.\n");
    }
    printf("TESTED\n");

    for(MessageInfo msg;;){
        if (Message::receive(&msg)) continue;
        switch (msg.header){
        default:
            printf("Client default: %d", msg.header);
            break;
        }
        printf("loop\n");
    }
    printf("ClientExit\n");
    return 0;
}

int NetClient::initalize(dword threadid)
{
    serverid=threadid; 
    clientid=System::getThreadID();
    return 0;
}
