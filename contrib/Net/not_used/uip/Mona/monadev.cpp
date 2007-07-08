#include <uip.h>
#include "NicServer.h"
#include "Util.h"

using namespace mones;

extern uint32_t nic_read(uint32_t nicThread, Ether::Frame* frame);
extern uint32_t nic_write(uint32_t nicThread, OutPacket* packet);
extern uint32_t nicThread;


void
monadev_init(void)
{
    return;
}

unsigned int
monadev_read(void)
{
    Ether::Frame frame;
    if (1 == nic_read(nicThread, &frame)) return 0;
    memcpy(uip_buf, &frame, UIP_BUFSIZE);
    return UIP_BUFSIZE;
}

static OutPacket p;

void
monadev_send(void)
{
    int ret;
    char tmpbuf[UIP_BUFSIZE];
    int i;
    for(i = 0; i < 40 + UIP_LLH_LEN; i++) {
        tmpbuf[i] = uip_buf[i];
    }

    for(; i < uip_len; i++) {
        tmpbuf[i] = uip_appdata[i - 40 - UIP_LLH_LEN];
    }
    memcpy(p.header, tmpbuf, uip_len);

    Ether::Frame* frame = (Ether::Frame*)p.header;
    Arp::Header* header = (Arp::Header*)frame->data;
    memcpy(p.destmac, frame->dstmac, 6);
    p.protocol = Util::swapShort(frame->type);
    p.size = uip_len;

    ret = nic_write(nicThread, &p);
    return;
}
