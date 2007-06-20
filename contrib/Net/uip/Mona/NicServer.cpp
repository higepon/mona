#include "NicServer.h"

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

    syscall_set_irq_receiver(this->nic->getIRQ(), false);
    monapi_set_irq(this->nic->getIRQ(), MONAPI_TRUE, MONAPI_TRUE);
    this->nic->enableNetwork();
    this->nic->getMacAddress(this->macAddress);
    this->observerThread= Message::lookupMainThread();
    this->myID = System::getThreadID();
    this->started = true;
    return true;
}

uint32_t NicServer::getThreadID() const
{
    return this->myID;
}

void NicServer::interrupt(MessageInfo* msg)
{
    MessageInfo info;
    Message::create(&info, MSG_FRAME_READY, 0, 0, 0, NULL);
    this->nic->inputFrame();
    Ether::Frame* frame = new Ether::Frame;
    this->nic->getFrameBuffer((uint8_t*)frame, sizeof(Ether::Frame));
    this->frameList.add(frame);
    if (Message::send(this->observerThread, &info)) {
        printf("local!!!! yamas:INIT error\n");
    }
    return;
}

bool NicServer::WaitIntteruptWithTimeout(MessageInfo* msg)
{
    MessageInfo m;
    uint32_t timerId = set_timer(100);
    for (int i = 0; ; i++)
    {
        int result = MonAPI::Message::peek(&m, i);

        if (result != 0)
        {
            i--;
            syscall_mthread_yield_message();
        }
        else if (m.header == MSG_TIMER)
        {
            if (m.arg1 != timerId) continue;
            kill_timer(timerId);
            Message::peek(&m, i, PEEK_REMOVE);

            MonAPI::Message::reply(msg, 1);
            return false;
        }
        else if (m.header == MSG_INTERRUPTED)
        {
            kill_timer(timerId);
            MonAPI::Message::peek(&m, i, PEEK_REMOVE);
            interrupt(&m);
            return true;
        }
    }
    return false;
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
            if (this->frameList.size() == 0)
            {
                if (!WaitIntteruptWithTimeout(&msg)) {
                    // timeout
                    break;
                }
            }

            Ether::Frame* frame = this->frameList.removeAt(0);
            SetFrameToSharedMemory(frame);
            delete frame;
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
