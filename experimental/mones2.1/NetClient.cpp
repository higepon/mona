#include "NetClient.h"

using namespace MonAPI;
using namespace mones;

/*
typedef struct {
    dword header;
    dword arg1;
    dword arg2;
    dword arg3;
    dword from;
    char str[128];
    int length;
} MessageInfo;

class{
    word  protocol;
    word  localport;
    word  remoteport;
    dword remoteip; //dcba
    dword clientid;
}
*/

int NetClient::Open() //remoteip,remoteport,localport,protocol
{
    Message::create(&info,MSG_NET_OPEN, 0, 0, 0, NULL);
    if (Message::send(serverid, &info)) {
        printf("MSG_NET_OPEN error\n");
    }
    return Peek(MSG_NET_OPEN);
}

int NetClient::Close()
{
    Message::create(&info,MSG_NET_CLOSE, 0, 0, 0, NULL);
    if (Message::send(serverid, &info)) {
        printf("MSG_NET_COLSE error\n");
    }
    return Peek(MSG_NET_CLOSE);
}
int NetClient::Read() //data
{   
    Message::create(&info,MSG_NET_READ, 0, 0, 0, NULL);
    if (Message::send(serverid, &info)) {
        printf("MSG_NET_READ error\n");
    }
    return Peek(MSG_NET_READ);
}
int NetClient::Write() //data,size,
{
    Message::create(&info,MSG_NET_WRITE, 0, 0, 0, NULL);
    if (Message::send(serverid, &info)) {
        printf("MSG_NET_WRITE error\n");
    }
    return Peek(MSG_NET_WRITE);
}

int NetClient::Stat()//data
{
    Message::create(&info,MSG_NET_STATUS, 0, 0, 0, NULL);
    if (Message::send(serverid, &info)) {
        printf("MSG_NET_STATUS error\n");
    }
    return Peek(MSG_NET_STATUS);
}

int NetClient::Peek(dword msg)
{
    for (int i = 0; ; i++){
        int result = Message::peek(&info, i);
        if (result != 0){
            i--;
            syscall_mthread_yield_message();
        } else if (info.header == MSG_RESULT_OK && info.arg1 == msg ) {
            MonAPI::Message::peek(&info, i, PEEK_REMOVE);
            return 0;
        } 
    }
}

int NetClient::Test()
{
    sleep(1000);
    if( Open( ) ){
        printf("OpenError.\n");
    }
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

 