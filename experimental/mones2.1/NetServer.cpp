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
            this->Interrupt(&msg);   
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
    ConnectionInfo* c=NULL;
    switch(msg->arg3)
    {
    case TYPEICMP:
        ICMPCoInfo* pI=new ICMPCoInfo(this);
        pI->type=ECHOREQUEST;
        pI->seqnum=0;
        pI->idnum=0;
        cinfolist.add(pI);
        c=pI;
        break;
    case TYPEUDP:
        UDPCoInfo* pU = new UDPCoInfo(this);
        cinfolist.add(pU);
        c=pU;
        break;
    case TYPETCP:
        TCPCoInfo* pT=new TCPCoInfo(this);
        cinfolist.add(pT);
        c=pT;
        break;
    default:
        printf("orz\n");
    }
    c->remoteip   = msg->arg1; 
    c->remoteport = (word)(msg->arg2&0x0000FFFF);
    c->localport  = (word)(msg->arg2>>16);
    //c->protocol   = msg->arg3;
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
    readStatus(&stat);
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
    //printf("%d",syscall_get_tick());
    PeriodicUpdate();
}

void NetServer::Interrupt(MessageInfo* msg)
{   
    //Don't say anything about in case mona is a router.
    int val = interrupt();
    if( val & Nic::RX_INT ){
      //  printf("=RX\n");
        DoDispatch();
    }
    if(val & Nic::TX_INT){
      //  printf("=TX\n");
    }
    if( val & Nic::ER_INT){
      //  printf("=ERROR.\n");    
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
    //printf("READING\n");
    DoDispatch();
}

void NetServer::write(MessageInfo* msg)
{ 
    monapi_cmemoryinfo* ret = monapi_cmemoryinfo_new();
    if( ret != NULL){
        ret->Handle = msg->arg2;
        ret->Owner  = msg->from;
        ret->Size   = msg->arg3;
        monapi_cmemoryinfo_map(ret);
        for(int i=0; i<cinfolist.size(); i++){
            ConnectionInfo* cinfo  = cinfolist.get(i);
            if( cinfo->netdsc == msg->arg1 ){    
                Send(ret->Data,ret->Size,cinfo);
                break;
            }
        }
        monapi_cmemoryinfo_delete(ret);
    }
    Message::reply(msg);  
}
