
#include "NicServer.h"
#include "Util.h"

using namespace mones;

extern dword nic_read(dword nicThread, Ether::Frame* frame);
extern dword nic_write(dword nicThread, OutPacket* packet);
extern dword nicThread;


void
monadev_init(void)
{
    return;
}

unsigned int
monadev_read(void)
{
    Ether::Frame frame;
    nic_read(nicThread, &frame);
    //memcpy(uip_buf, &frame, UIP_BUFSIZE);
    //return UIP_BUFSIZE;
	return 0;
}

static OutPacket p;

void
monadev_send(void)
{
    int ret;


    ret = nic_write(nicThread, &p);
    return;
}
