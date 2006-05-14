#include "NicServer.h"
#include "Util.h"

using namespace mones;
using namespace MonAPI;

// not real shared memory
static Ether::Frame sharedFrame;

void mones::SetFrameToSharedMemory(Ether::Frame* frame)
{
    memcpy(&sharedFrame, frame, sizeof(Ether::Frame));
}
void mones::GetFrameFromSharedMemory(Ether::Frame* frame)
{
    memcpy(frame, &sharedFrame, sizeof(Ether::Frame));
}

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
    this->observerThread= Message::lookupMainThread();
    this->myID = System::getThreadID();
    this->started = true;
    return true;
}

dword NicServer::getThreadID() const
{
    return this->myID;
}

int NicServer::ARPhandler(Ether::Frame* frame)
{
    //handling ARP without message mechanism.
    printf("ARP");
    Arp::Header* header=(Arp::Header*)(frame->data);
    if( header->dstIp == nic->getIP() ){
        header->opeCode=Util::swapShort(Arp::OPE_CODE_ARP_REP);
        memcpy(header->dstMac,header->srcMac,6);
        nic->getMacAddress(header->srcMac);
        header->dstIp=header->srcIp;
        header->srcIp=nic->getIP();
        memcpy(frame->dstmac,header->dstMac,6);
        memcpy(frame->srcmac,header->srcMac,6);
        nic->Send(frame);
    }else{
        delete frame;
    }    
    return 0;
}

void NicServer::interrupt(MessageInfo* msg)
{   
    //Don't say anything about in case mona is a router.
    int val = nic->interrupt();
    if( val & Nic::RX_INT ){
        Ether::Frame* frame =NULL;
        while( frame = nic ->Recv(0) ){
            if( Util::swapShort(frame->type) ==  Ether::ARP ){
                ARPhandler(frame);
            }else{
                IP::Header* header=(IP::Header*)(frame->data);
                printf("%x ",header->prot);
                for(int j=0;j<4;j++)
                    printf("%d.",*(((byte*)&(header->srcip))+j));
                for(int j=0;j<4;j++)
                    printf("%d.",*(((byte*)&(header->dstip))+j));
                printf("\n");
                delete frame;
            }
        }
    }
    if(val & Nic::TX_INT){
        printf("==TX\n");
    }
    if( val & Nic::ER_INT){
        printf("==ERROR.\n");    
    }
 //   MessageInfo info;
 //   Message::create(&info, MSG_FRAME_READY, 0, 0, 0, NULL);
 //   if(Message::send(this->observerThread, &info)) {
 //      printf("local!!!! yamas:INIT error\n");
 //   }
    return;
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
        case MSG_GET_MAC_ADDRESS:
        {
            memcpy(msg.str, this->macAddress, 6);
            MonAPI::Message::reply(&msg, 0, 0, msg.str);
            break;
        }
        default:
            printf("default come %d", msg.header);
            break;
        }
    }
    printf("NicServer exit\n");
}
