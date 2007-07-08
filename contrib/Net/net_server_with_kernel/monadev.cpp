#include <uip.h>
#include "Util.h"
#include "monadev.h"

void monadev_init(void)
{
    return;
}

unsigned int
monadev_read(void)
{
    Ether::Frame frame;
    if (1 == nic_read(&frame)) return 0;
    memcpy(uip_buf, &frame, UIP_BUFSIZE);
    logprintf("monadev_read end\n");
    return UIP_BUFSIZE;
}

void monadev_send(void)
{
    static OutPacket p;
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

    ret = nic_write(&p);
    return;
}

uint32_t nic_read(Ether::Frame* frame)
{
   int ret = syscall_receive_packet((uint8_t*)frame);
   if (1 == ret)
   {
       sleep(30);
   }
   return ret;
}

uint32_t nic_write(OutPacket* packet)
{
    syscall_send_packet(packet->header, packet->destmac, packet->size, packet->protocol);
    return 0;
}
