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
    syscall_set_irq_receiver(this->nic->getIRQ(), 0); // with mask Interrrupt
    this->nic->enableNetwork();
//  this->nic->getMacAddress(this->macAddress);
    this->observerThread= Message::lookupMainThread();
    this->myID = System::getThreadID();
    this->started = true;
    return true;
}

dword NicServer::getThreadID() const
{
    return this->myID;
}

int NicServer::AWKreply()
{
	//handling Arp and Well Known services without message mechanism.
	//Don't say anything about in case mona is a router.
	int i=0;
	Ether::Frame* frame = NULL;
	while( frame = nic->frameList.get(i) ){
		if( Util::swapShort(frame->type) ==  Ether::ARP ){
			nic->frameList.removeAt(i);
			printf("ARP\n");
			Arp::Header* header=(Arp::Header*)(frame->data);
			printf("%d %d %d %d %d\n",
            header->hardType,
			header->protType,
            header->hardAddrLen,
			header->protAddrLen,
			header->opeCode);
			for(int j=0;j<6;j++)
				printf("%x:",header->srcMac[j]);	
			for(int j=0;j<4;j++)
				printf("%d.",*(((byte*)&(header->srcIp))+j));
			printf("\n");
			for(int j=0;j<6;j++)
				printf("%x:",header->dstMac[j]);
			for(int j=0;j<4;j++)
				printf("%d.",*(((byte*)&(header->dstIp))+j));
			printf("\n");
			//send reply nic->send(frame);
			delete frame;
		}else{
			printf("IP\n");
			i++;
		}
	}
    while( nic->frameList.size() != 0){
        frame = nic ->frameList.removeAt(0);
		IP::Header* header=(IP::Header*)(frame->data);
		printf("%x ",header->prot);
		for(int j=0;j<4;j++)
			printf("%d.",*(((byte*)&(header->srcip))+j));
		for(int j=0;j<4;j++)
			printf("%d.",*(((byte*)&(header->dstip))+j));
		printf("\n");
		delete frame;
	}
	return 0;
}

void NicServer::interrupt(MessageInfo* msg)
{	
	int val = nic->interrupt();
	if( val & Nic::RX_INT ){
		AWKreply();
	}
	if(val & Nic::TX_INT){
        printf("==TX\n");
	}
	if( val & Nic::ER_INT){
        printf("==ERROR.\n");	
	}
    MessageInfo info;
    Message::create(&info, MSG_FRAME_READY, 0, 0, 0, NULL);
    if(Message::send(this->observerThread, &info)) {
       printf("local!!!! yamas:INIT error\n");
    }
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
            OutPacket* p = (OutPacket*)msg.arg1;
            this->nic->outputFrame(p->header, p->destmac, p->size, p->protocol);
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
