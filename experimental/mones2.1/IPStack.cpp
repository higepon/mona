//$Id$
#include "IPStack.h"
#include "ICMPInfo.h"
#include "UDPInfo.h"
#include "TCPInfo.h"

using namespace mones;
using namespace MonAPI;

IPStack::IPStack() : 
    next_port(0),timerid(0),
    started(false), loopExit(false)
{
    syscall_get_io();  
    this->myID = System::getThreadID();
    timerid=set_timer(5000);    
    pDP= new Dispatch();
    if(!pDP->initialize()){
        printf("initalize failed\n");
        delete pDP;
        exit(1);
    }
}

IPStack::~IPStack()
{
    if( pDP!=NULL )
        delete pDP;
    if( timerid != 0)
        kill_timer(timerid);
}

void IPStack::messageLoop()
{
    this->started = true;
    for (MessageInfo msg; !loopExit;){
        if (Message::receive(&msg)) continue;
        switch (msg.header)
        {
        case MSG_INTERRUPTED:
            pDP->interrupt();
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
        case MSG_NET_GETSTATUS:
            this->getstatus(&msg);
            break;  
        case MSG_NET_CONFIG:
            this->config(&msg);
            break;
        case MSG_NET_ICMPOPEN:
            this->icmpopen(&msg);
            break;
        case MSG_NET_UDPOPEN:
            this->udpopen(&msg);
            break;
        case MSG_NET_ACTVOPEN:
            this->tcpactvopen(&msg);
            break;
        case MSG_NET_PASVOPEN:
            this->tcppasvopen(&msg);
            break;
        case MSG_NET_ACCEPT:
            this->tcpaccept(&msg);
            break;
        case MSG_NET_CLOSE:
            this->close(&msg);
            break;
        case MSG_NET_RESET:
            this->reset(&msg);
            break;
        case MSG_NET_SETBLKMODE:
            this->setblockingmode(&msg);
            break;
        case MSG_TIMER:    
            pDP->PeriodicUpdate();
            break;
        default:
            printf("IPStack::ERROR MSG=%x\n", msg.header);
            break;
        }
    }
    printf("IPStack exit\n");
}

//////////// MESSAGE HANDLERS ////////////////////////

void IPStack::config(MessageInfo* msg)
{
    //IFCONFIG
    printf("re-configure the net server.\n");
    Message::reply(msg);
}

void IPStack::getfreeport(MessageInfo* msg)
{
    next_port++;
    if( next_port < 4096)
        next_port= 4096;
    Message::reply(msg,next_port);
}

void IPStack::getstatus(MessageInfo* msg)
{
    //ARP NETSTAT IFCONFIG
    NetStatus stat;
    pDP->readStatus(&stat);
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

void IPStack::icmpopen(MessageInfo* msg)
{
    int netdsc= pDP->getSerialNo();
    ICMPCoInfo* pI=new ICMPCoInfo(pDP);
    pI->type=ECHOREQUEST;
    pI->Init(msg->arg1, (word)(msg->arg2>>16),(word)(msg->arg2&0x0000FFFF), msg->from, netdsc);
    pDP->AddInfo(pI);
    Message::reply(msg, netdsc);
}

void IPStack::udpopen(MessageInfo* msg)
{
    int netdsc= pDP->getSerialNo();
    UDPCoInfo* pU = new UDPCoInfo(pDP);
    pU->Init(msg->arg1, (word)(msg->arg2>>16),(word)(msg->arg2&0x0000FFFF), msg->from, netdsc);
    pDP->AddInfo(pU);  
    Message::reply(msg, netdsc);
}

void IPStack::reset(MessageInfo* msg)
{
    TCPCoInfo T(pDP);
    T.Reset(msg->arg1, (word)(msg->arg2>>16),(word)(msg->arg2&0x0000FFFF));
    Message::reply(msg);
}

void IPStack::tcppasvopen(MessageInfo* msg)
{    
    int netdsc= pDP->getSerialNo();
    TCPCoInfo* pT=new TCPCoInfo(pDP);    
    pT->Init(0, (word)(msg->arg1),0, msg->from, netdsc);
    pDP->AddInfo(pT);
    pT->PasvOpen();
    memcpy(&(pT->msg),(byte*)msg,sizeof(MessageInfo)); //Register msg.
    Message::reply(msg, netdsc);

}
void IPStack::tcpactvopen(MessageInfo* msg)
{
    int netdsc= pDP->getSerialNo();
    TCPCoInfo* pT=new TCPCoInfo(pDP);    
    pT->Init(msg->arg1, (word)(msg->arg2>>16),(word)(msg->arg2&0x0000FFFF), msg->from, netdsc);
    pDP->AddInfo(pT);
    pT->ActvOpen();
    memcpy(&(pT->msg),(byte*)msg,sizeof(MessageInfo)); //Register msg.
}

void IPStack::tcpaccept(MessageInfo* msg)
{
    for(int i=0;i<pDP->InfoNum();i++){
        TCPCoInfo* pT  = (TCPCoInfo*)(pDP->GetInfo(i));
        if( pT->netdsc == msg->arg1 ){
            pT->Accept(msg);
            break;
        }
    } 
}

void IPStack::close(MessageInfo* msg)
{
    dword ret=1;
    for(int i=0; i< pDP->InfoNum(); i++){
        L4Base* c  = pDP->GetInfo(i);
        if( c->netdsc == msg->arg1 ){
            c->Close();
            ret=0;
        }
    }
    Message::reply(msg,ret);
}

void IPStack::read(MessageInfo* msg)
{
    //Register msg to waiting client list.
    for(int i=0; i<pDP->InfoNum(); i++){
        L4Base* c  = pDP->GetInfo(i);
        if( c->netdsc == msg->arg1 ){
            c->Read(msg);
            break;
        }
    }
    pDP->DoDispatch();
}

void IPStack::write(MessageInfo* msg)
{ 
    for(int i=0;i<pDP->InfoNum();i++){
        L4Base* c  = pDP->GetInfo(i);
        if( c->netdsc == msg->arg1 ){
            c->Write(msg);
            break;
        }
    } 
}

void IPStack::setblockingmode(MessageInfo* msg)
{
    for(int i=0;i<pDP->InfoNum();i++){
        L4Base* c  = pDP->GetInfo(i);
        if( c->netdsc == msg->arg1 ){
            c->SetBlockingMode(msg);
            break;
        }
    } 
    Message::reply(msg);
}
