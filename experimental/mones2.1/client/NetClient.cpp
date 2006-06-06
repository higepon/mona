//$Id$
#include "NetClient.h"
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
    if (Message::sendReceive(&msg, serverid, MSG_NET_STATUS) != 0){
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
    sleep(500);
    //read nic status.
    NetStatus stat;
    if( Stat(&stat) ){
        printf("\n%s:\nIPaddress:",stat.devname);
        for(int j=0;j<4;j++)
            printf("%d.",*(((byte*)&stat.localip)+j));
        printf("NetMask:");
        for(int j=0;j<4;j++)
            printf("%d.",*(((byte*)&stat.netmask)+j));
        printf("\nDefaultRoute:");
        for(int j=0;j<4;j++)
            printf("%d.",*(((byte*)&stat.defaultroute)+j));
        printf("MTU:%d MAC:",stat.mtu);
        char buf[16];
        for(int j=0;j<6;j++){
            sprintf(buf,"%02x",stat.mac[j]);
            printf("%s:",buf);
        }
        printf("\n");
    }
    dword remoteip=(3<<24)|(10<<16)|(168<<8)|(192);
    if( stat.localip==LOOPBACKIP ){
        remoteip=LOOPBACKIP;
    }
    ///////////////////////////////////////////////
    printf("\nSend ICMP echo request\n");
    int netdsc= Open(remoteip,0,0,TYPEICMP);
    if( netdsc< 0 ){
        printf("OpenError.\n");
    }
    printf("Open::netdsc=%d\n",netdsc);
    if( Write(netdsc,(byte*)"How are you?",12) ){
        print("WriteError\n");
    }
    byte buf[1024];//BAD design.
    int size= Read(netdsc,buf);
    if( size > 0 ){
        //printf("Read: %s\n",buf);
        if(!strcmp((char*)buf,"How are you?")){
            printf("destination is Alive.\n");
        }
        //if recived string is same as sended.
        //printf("dset is alive,\n");
    }
    if( Close(netdsc) ){
        printf("CloseError.\n");
    }   
    ////////////////////////////////////////////
    printf("\nSend UDP to DAYTIME\n");    
    word localport = GetFreePort();
    printf("Port=%d\n",localport);
    netdsc = Open(remoteip,localport,DAYTIME,TYPEUDP);
    if( netdsc < 0 ){
        printf("OpenError.\n");
    }
    printf("Open::netdsc=%d\n",netdsc);
    
    if( Write(netdsc,(byte*)"What time is it now?",20) ){
        printf("WrieError.\n");
    }
    size= Read(netdsc,buf);
    if( size > 0 ){
        printf("remote time is %s\n",buf);
    }    
    if( Close(netdsc) ){
        printf("CloseError.\n");
    }    
    //////////////////////////////////////////////////

    //re-setup nic. 
    char devname[]="pcnet0";
    if( Config(devname,(5<<24)|(0<<16)|(168<<8)|192,(1<24)|(0<<16)|(168<<8)|192,24,60,1500) ){
        printf("ConfigError\n");
    }
    printf("tests are completed.\n");
    return 0;
}

int NetClient::initalize(dword threadid)
{
    serverid=threadid;
    printf("serverid=%d\n",serverid);
    clientid=System::getThreadID();
    return 0;
}

///////////////////////////////////////////////////////////////////

int MonaMain(List<char*>* pekoe)
{
    NetClient* client = new NetClient(); 
    client->initalize(Message::lookupMainThread("MNS21.EX5"));
    client->Example();
    delete client;
    exit(0);
    return 0;
}
