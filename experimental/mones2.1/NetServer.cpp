#include "NetServer.h"
using namespace mones;
using namespace MonAPI;

NetServer::NetServer() : 
    next_port(0), observerThread(0xffffffff),nic(NULL), started(false), loopExit(false)
{

}

NetServer::~NetServer()
{

}

bool NetServer::initialize()
{
    syscall_get_io();
    this->nic = NicFactory::create();
    if(this->nic == NULL){
        printf("NicFactory error\n");
        return false;
    }
    syscall_set_irq_receiver(this->nic->getIRQ(), SYS_MASK_INTERRUPT); // with mask Interrrupt by higepon
    this->nic->enableNetwork();
    this->ipstack = new IPStack();
    dword ip= nic->getIP();         
    for(int j=0;j<4;j++)
          printf("%d.",*(((byte*)&ip)+j));
    printf("\n");
    this->observerThread= Message::lookupMainThread();
    this->myID = System::getThreadID();
    return true;
}

dword NetServer::getThreadID() const
{
    return this->myID;
}

void NetServer::ICMPreply(IP* pkt)
{
    Ether* rframe = nic->MakePKT(pkt->srcip);
    //ether header has been already filled.
    if( rframe != NULL){
        ICMP* icmp=rframe->IPHeader->ICMPHeader;
        //FillICMPHeader();
        //create ICMP echo reply.
        icmp->type=0x00;
        icmp->code=0x00;
        icmp->chksum=pkt->ICMPHeader->chksum+8;
        memcpy(icmp->data,pkt->ICMPHeader->data,bswap(pkt->len)-24);
        printf("replying.\n");    
        ipstack->FillIPHeader(rframe->IPHeader);
        nic->Send(rframe);
    }
}
void NetServer::messageLoop()
{
    this->started = true;
    for (MessageInfo msg; !loopExit;){
        if (Message::receive(&msg)) continue;
        switch (msg.header){
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
            Message::reply(&msg);
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

////////////MESSAGE HANDLERS///////////////////////////////////////////

void NetServer::getfreeport(MessageInfo* msg)
{
    next_port++;
    if( next_port <= 0x400)
        next_port=0x401;
    Message::reply(msg,next_port);
}

void NetServer::open(MessageInfo* msg)
{    
    CNI* c=new CNI();
    connectlist.add(c);
    c->remoteip   = (word)((msg->arg1)>>16);
    c->remoteport = (word)(msg->arg2|0xFFFF);
    c->localport  = msg->arg2;
    c->protocol   = msg->arg3;
    c->clientid   = msg->from;    
    c->netdsc     = connectlist.size();    
    Message::reply(msg, c->netdsc);
}

void NetServer::close(MessageInfo* msg)
{
    dword ret=1;
    int n= connectlist.size();
    for(int i= 0;i<n;i++){
        CNI* c  = connectlist.get(i);
        if( c->netdsc == msg->arg1 ){
            delete connectlist.removeAt(i);
            n--;
            ret=0;
        }
    }
    Message::reply(msg,ret);
}

void NetServer::status(MessageInfo* msg)
{
    //msg.str;
    NetStatus stat;
    stat.a=5678;
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

void NetServer::interrupt(MessageInfo* msg)
{   
    //Don't say anything about in case mona is a router.
    int val = nic->interrupt();
    if( val & Nic::RX_INT ){
        printf("==RX\n");
        Ether* frame =NULL;
        while( frame = nic ->Recv(0) ){
            IP* pkt=frame->IPHeader;
            ipstack->dumpPacket(pkt);
            if( pkt->prot == TYPEICMP){
                ICMPreply(pkt);
            }
            //unblock
            delete frame;
        }
    }
    if(val & Nic::TX_INT){
        printf("==TX\n");
    }
    if( val & Nic::ER_INT){
        printf("==ERROR.\n");    
    }
}


void NetServer::read(MessageInfo* msg)
{
    /////
    byte val[]="string";   
    printf("noblock=%d\n",msg->arg1);
    ///////
    monapi_cmemoryinfo* mi = monapi_cmemoryinfo_new();  
    if (mi != NULL){    
        monapi_cmemoryinfo_create(mi,7, true);        
        if( mi != NULL ){
            memcpy(mi->Data,val,mi->Size);mi->Data[6]='\0';
            Message::reply(msg, mi->Handle, mi->Size); 
        }
        monapi_cmemoryinfo_delete(mi);
    }else{
        Message::reply(msg);
    }
}

void NetServer::write(MessageInfo* msg)
{
    monapi_cmemoryinfo* ret = monapi_cmemoryinfo_new();
    if( ret != NULL){
        ret->Handle = msg->arg1;
        ret->Owner  = msg->from;
        ret->Size   = msg->arg2;
        monapi_cmemoryinfo_map(ret);
        byte* buf=new byte[ret->Size+1];
        memcpy(buf,ret->Data,ret->Size); buf[ret->Size]='\0';
        printf("<%s>\n",buf);
        monapi_cmemoryinfo_delete(ret);
        delete buf;
    }
    Message::reply(msg);  
}




