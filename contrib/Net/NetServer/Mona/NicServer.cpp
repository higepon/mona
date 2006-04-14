#include "NicServer.h"
#include <monapi/Thread.h>
using namespace mones;
using namespace MonAPI;
using namespace std;

// not real shared memory
static Ether::Frame sharedFrame;
static Mutex sharedMutex;

void mones::SetFrameToSharedMemory(Ether::Frame* frame)
{
    sharedMutex.lock();
    memcpy(&sharedFrame, frame, sizeof(Ether::Frame));
    sharedMutex.unlock();
}
void mones::GetFrameFromSharedMemory(Ether::Frame* frame)
{
    sharedMutex.lock();
    memcpy(frame, &sharedFrame, sizeof(Ether::Frame));
    sharedMutex.unlock();
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
    if(this->nic == NULL) 
    {
        printf("NicFactory error\n");
        return false;
    }
    syscall_set_irq_receiver(this->nic->getIRQ());
    this->nic->enableNetwork();
    this->nic->getMacAddress(this->macAddress);
    this->observerThread= Message::lookupMainThread();
    this->myID = System::getThreadID();
    this->started = true;
    return true;
}

dword NicServer::getThreadID() const
{
    return this->myID;
}

#if 1 /* for Mona */

#pragma pack(2)
    typedef struct{
        byte  dstmac[6];   // 送信先 MAC ID
        byte  srcmac[6];   // 送信元 MAC ID
        word    type;     // フレームタイプ Frame type(DIX) or frame length(IEEE)
        byte   data[0x600];// Data
    } Frame;

#pragma pack(2)
    typedef struct{
        byte  verhead;  /* バージョン、ヘッダ長。 */
        byte  tos;      /* TOS. */
        word len;       /* トータル長。 */
        word id;        /* 識別番号。 */
        word frag;      /* フラグ、フラグメントオフセット。 */
        byte  ttl;      /* Time to Live. */
        byte  prot;     /* プロトコル番号。 */
        word chksum;    /* ヘッダチェックサム。 */
        dword srcip;        /* 送り元IP。 */
        dword dstip;        /* 宛先IP。 */
        char     data[0];
    } IPHeader;
#pragma pack(0)
#pragma pack(2)
typedef struct
{
  word src;                    /* source port */
  word dst;                    /* destination port */
  dword seq_number;             /* sequence number */
  dword ack_number;             /* acknowledgement number */
#ifdef WORDS_BIGENDIAN
  byte  header_length:4,        /* data offset */
            unused:4;               /* (unused) */
#else
  byte  unused:4,               /* (unused) */
            header_length:4;        /* data offset */
#endif
  byte  flags;
  word window;                 /* window */
  word checksum;               /* checksum */
  word urgent;                 /* urgent pointer */
} TCPHeader;
#pragma pack(0)
    inline static dword swapLong(dword value)
    {
        return (value>>24)+((value>>8)&0xff00)+((value<<8)&0xff0000)+(value<<24);
    }

    inline static word swapShort(word value)
    {
        return (value>>8)+(value<<8);
    }
#define TCP_SYN 0x02
#define TCP_ACK 0x10

#endif

void NicServer::interrupt(MessageInfo* msg)
{
    MessageInfo info;
    Message::create(&info, MSG_FRAME_READY, 0, 0, 0, NULL);
    this->nic->inputFrame();
    Ether::Frame* frame = new Ether::Frame;
    this->nic->getFrameBuffer((byte*)frame, sizeof(Ether::Frame));
#if 1
    if (frame->type == 0x8) { // IP
        const IPHeader* h = (const IPHeader*)frame->data;
        if (h->prot == 6) // TCP
        {
            const TCPHeader* th = (const TCPHeader*)h->data;
            int ack = th->flags & TCP_ACK;
            int syn = th->flags & TCP_SYN;

            logprintf("NICSERVER:%d to %d:len=%4d %s%s seqno=%08x ackno=%08x\n", swapShort(th->src), swapShort(th->dst)
                   , th->header_length, ack ? "ACK " : "", syn ? "SYN " : "   ", swapLong(th->seq_number),swapLong(th->ack_number));
        } else {
            logprintf("not tcp\n");
        }
    } else {
        logprintf("not ip\n");
    }
#endif
//    this->frameList.add(frame);
    frameQueue_.push(frame);
    if (Message::send(this->observerThread, &info)) {
        printf("local!!!! yamas:INIT error\n");
    }
    return;
}

bool NicServer::WaitIntteruptWithTimeout(MessageInfo* msg)
{
    MessageInfo m;
    dword timerId = set_timer(30); // 30msだとうまく行く
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
//            if (this->frameList.size() == 0)
            if (frameQueue_.empty())
            {
                if (!WaitIntteruptWithTimeout(&msg)) {
                    // timeout
                    break;
                }
            }

//            Ether::Frame* frame = this->frameList.removeAt(0);
            Ether::Frame* frame = frameQueue_.front();
            frameQueue_.pop();
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
