//$Id$
#include "NetServer.h"
using namespace mones;
using namespace MonAPI;

NetServer::NetServer() : 
    next_port(0),timerid(0),
    started(false), loopExit(false)
{
    syscall_get_io();  
    this->myID = System::getThreadID();
    timerid=set_timer(5000);
}

NetServer::~NetServer()
{
    if( timerid != 0)
        kill_timer(timerid);
}


dword NetServer::getThreadID() const
{
    return this->myID;
}

void NetServer::messageLoop()
{
    this->started = true;
    for (MessageInfo msg; !loopExit;){
        if (Message::receive(&msg)) continue;
        switch (msg.header)
        {
        case MSG_INTERRUPTED:
            this->interrupt(&msg);   
            break;
        case MSG_NET_GETFREEPORT:
            this->getfreeport(&msg);
            break;
        case MSG_NET_WRITE:
            this->write(&msg);
            break;
        case MSG_NET_READ:
            this->read(&msg);
            break;
        case MSG_NET_STATUS:
            this->status(&msg);
            break;
        case MSG_NET_OPEN:
            this->open(&msg);
            break;
        case MSG_NET_CLOSE:
            this->close(&msg);
            break;
        case MSG_NET_CONFIG:
            this->config(&msg);
            break;
        case MSG_TIMER:
            this->ontimer(&msg);
            break;
        default:
            printf("NetServer::MessageLoop default MSG=%x\n", msg.header);
            break;
        }
    }
    printf("NetServer exit\n");
}

/*typedef struct {
    dword header;
    dword arg1;
    dword arg2;
    dword arg3;
    dword from;
    char str[128];
    int length;
} MessageInfo; */

//////////// MESSAGE HANDLERS ////////////////////////
void NetServer::config(MessageInfo* msg)
{
    printf("re-configure the net server.\n");
    Message::reply(msg);
}

void NetServer::getfreeport(MessageInfo* msg)
{
    next_port++;
    if( next_port <= 0x400)
        next_port=0x401;
    Message::reply(msg,next_port);
}

void NetServer::open(MessageInfo* msg)
{    
    ConnectionInfo* c=new ConnectionInfo();
    cinfolist.add(c);
    c->Id.remoteip   = (word)((msg->arg1)>>16);
    c->Id.remoteport = (word)(msg->arg2|0xFFFF);
    c->Id.localport  = msg->arg2;
    c->Id.protocol   = msg->arg3;
    c->clientid      = msg->from;    
    c->netdsc     = cinfolist.size();  
    c->msg.header = 0x0;
    Message::reply(msg, c->netdsc);
}

void NetServer::close(MessageInfo* msg)
{
    dword ret=1;
    for(int i=0; i<cinfolist.size(); i++){
        ConnectionInfo* c  = cinfolist.get(i);
        if( c->netdsc == msg->arg1 ){
            delete cinfolist.removeAt(i);
            i--;
            ret=0;
        }
    }
    Message::reply(msg,ret);
}

void NetServer::status(MessageInfo* msg)
{
    NetStatus stat;
    nic->getStatus(&stat);
    monapi_cmemoryinfo* mi = monapi_cmemoryinfo_new();  
    if (mi != NULL){
        monapi_cmemoryinfo_create(mi, sizeof(NetStatus)/*+sizeof(arpcache)*N*/, true);        
        if( mi != NULL ){
            memcpy(mi->Data,&stat,mi->Size);
            Message::reply(msg, mi->Handle, mi->Size);
        }
        monapi_cmemoryinfo_delete(mi);
    }else{
        Message::reply(msg);
    }
}

void NetServer::ontimer(MessageInfo* msg)
{
   //printf("TIMER\n");
}

void NetServer::interrupt(MessageInfo* msg)
{   
    //Don't say anything about in case mona is a router.
    int val = nic->interrupt();
    if( val & Nic::RX_INT ){
        printf("=RX\n");
        Dispatch();
    }
    if(val & Nic::TX_INT){
        printf("=TX\n");
    }
    if( val & Nic::ER_INT){
        printf("=ERROR.\n");    
    }
}

void NetServer::Dispatch()
{
    //TODO handling two or more packets. 
    Ether* frame = nic ->Recv(0);
    if( frame != NULL ){
        IP* pkt=frame->IPHeader;
        dumpPacket(pkt);
        if( pkt->prot == TYPEICMP && pkt->ICMPHeader->type==ECHOREQUEST){
            ICMPreply(pkt);
            delete frame;
            return;
        }
        ConnectionInfo* cinfo=NULL;
        //find a waiting client for current packt.
        for(int i=0; i<cinfolist.size(); i++){
            cinfo  = cinfolist.get(i);
            // if ( ipstack->Match((byte*)&(cinfo->Id),pkt ) ){
            break;
            // }
        }
        if( cinfo != NULL && cinfo->msg.header != 0x0 ){
            printf("noblock=%d\n",cinfo->msg.arg2);
            monapi_cmemoryinfo* mi = monapi_cmemoryinfo_new();  
            if (mi != NULL){
                int size=7;
                monapi_cmemoryinfo_create(mi,size, true);        
                if( mi != NULL ){
                    memcpy(mi->Data,frame->IPHeader->UDPHeader->data,mi->Size);mi->Data[mi->Size]='\0';
                    Message::reply(&(cinfo->msg), mi->Handle, mi->Size); 
                }
                monapi_cmemoryinfo_delete(mi);
            }else{
                Message::reply(&(cinfo->msg));
            }
            memset(&(cinfo->msg),'\0',sizeof(MessageInfo));
            cinfo=NULL;
            delete frame;
            frame=NULL;
        }
        //very dirty design.
        if( frame != NULL )
            nic->rxFrameList.add(frame);
    }
}

void NetServer::read(MessageInfo* msg)
{
    //Register msg to waiting client list.
    for(int i=0; i<cinfolist.size(); i++){
        ConnectionInfo* c  = cinfolist.get(i);
        if( c->netdsc == msg->arg1 ){
            memcpy(&(c->msg),(byte*)msg,sizeof(MessageInfo));
            break;
        }
    }
    printf("READING\n");
    Dispatch();
}

void NetServer::write(MessageInfo* msg)
{ 
    monapi_cmemoryinfo* ret = monapi_cmemoryinfo_new();
    if( ret != NULL){
        ret->Handle = msg->arg2;
        ret->Owner  = msg->from;
        ret->Size   = msg->arg3;
        monapi_cmemoryinfo_map(ret);
        Ether* frame= new Ether();
        //create ether/IP/protocol headers
        memcpy(frame->IPHeader->UDPHeader->data,ret->Data,ret->Size); //buf[ret->Size]='\0';
        nic->Send(frame);
        monapi_cmemoryinfo_delete(ret);
    }
    Message::reply(msg);  
}
