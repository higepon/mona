#include "NicServer.h"

using namespace mones;
using namespace MonAPI;

NicServer::NicServer() : observerThread(0xffffffff), nic(NULL), started(false), loopExit(false)
{
}

NicServer::~NicServer(){
}

bool NicServer::initialize()
{
    syscall_get_io();

    this->nic = NicFactory::create();
    if(this->nic == NULL){
        printf("NicFactory error\n");
        return false;
    }
    syscall_set_irq_receiver(this->nic->getIRQ(), SYS_MASK_INTERRUPT); // with mask Interrrupt by higepon
    this->nic->enableNetwork();
    dword ip= nic->getIP();         
    for(int j=0;j<4;j++)
          printf("%d.",*(((byte*)&ip)+j));
    printf("\n");
    this->observerThread= Message::lookupMainThread();
    this->myID = System::getThreadID();
    this->started = true;
    return true;
}

dword NicServer::getThreadID() const
{
    return this->myID;
}

void NicServer::ICMPreply(IP* pkt)
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
        //printf("replying.\n");    
        FillIPHeader(rframe->IPHeader);
        nic->Send(rframe);
    }
}

void NicServer::interrupt(MessageInfo* msg)
{   
    //Don't say anything about in case mona is a router.
    int val = nic->interrupt();
    if( val & Nic::RX_INT ){
        //printf("==RX\n");
        Ether* frame =NULL;
        while( frame = nic ->Recv(0) ){
            IP* pkt=frame->IPHeader;
            //dumpPacket(pkt);
            if( pkt->prot == IP::TYPEICMP){
                ICMPreply(pkt);
            }
            delete frame;
        }
    }
    if(val & Nic::TX_INT){
        //printf("==TX\n");
    }
    if( val & Nic::ER_INT){
        printf("==ERROR.\n");    
    }
    return;
 //   MessageInfo info;
 //   Message::create(&info, MSG_FRAME_READY, 0, 0, 0, NULL);
 //   if(Message::send(this->observerThread, &info)) {
 //      printf("local!!!! yamas:INIT error\n");
 //   }
}


void NicServer::messageLoop()
{
    for (MessageInfo msg; !loopExit;)
    {
        if (Message::receive(&msg)) continue;

        switch (msg.header)
        {
        case MSG_INTERRUPTED:
        {
            this->interrupt(&msg);   
            break;
        }
        case MSG_FRAME_WRITE:
        {
            //OutPacket* p = (OutPacket*)msg.arg1;
            //this->nic->outputFrame(p->header, p->destmac, p->size, p->protocol);
            MonAPI::Message::reply(&msg);
            break;
        }
        case MSG_FRAME_READ:
        {
            /*
            if (this->frameList.size() == 0)
            {
                MessageInfo m;
                for (int i = 0; ; i++)
                {
                    int result = MonAPI::Message::peek(&m, i);

                    if (result != 0)
                    {
                        i--;
                        syscall_mthread_yield_message();
                    }
                    else if (m.header == MSG_INTERRUPTED)
                    {
                        printf("XX\n");
                        MonAPI::Message::peek(&m, i, PEEK_REMOVE);
                        interrupt(&m);
                        break;
                    }
                }
            }
            */
            //Ether::Frame* frame = this->frameList.removeAt(0);
            //SetFrameToSharedMemory(frame);
            //delete frame;    
            //printf("read\n");
            sleep(500);
            MonAPI::Message::reply(&msg);
            break;
        }
        default:
            printf("default come %d", msg.header);
            break;
        }
    }
    printf("NicServer exit\n");
}
