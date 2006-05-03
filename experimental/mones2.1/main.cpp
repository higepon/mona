#include "monadev.h"
#include "NicServer.h"
#include <monapi.h>
#include <monalibc.h>

using namespace mones;
static NicServer* server;
void NicListenLoop();
dword nic_read(dword nicThread, Ether::Frame* frame);
dword nic_write(dword nicThread, OutPacket* packet);

dword nicThread;

#define BUF ((struct uip_eth_hdr *)&uip_buf[0])

#ifndef NULL
#define NULL (void *)0
#endif /* NULL */
int MonaMain(List<char*>* pekoe)
{
    dword id = syscall_mthread_create((dword)NicListenLoop);
    syscall_mthread_join(id);

    for (;;) {
        if (server != NULL && server->isStarted()) {
            break;
        }
        sleep(500);
    }
    nicThread = server->getThreadID();
    //printf(">>%d\n",nicThread);
    byte i, arptimer;
    /* Initialize the device driver. */
    monadev_init();
    /* Initialize the uIP TCP/IP stack. */
    //uip_init();
    /* Initialize the HTTP server. */
    //httpd_init();
    arptimer = 0;

    while(1) {
		sleep(500);
  
        monadev_read();
    
    }
    return 0;
}
/*-----------------------------------------------------------------------------------*/

void NicListenLoop()
{
    server = new NicServer;
    if (!server->initialize())
    {
        printf("NicServer initialize failed\n");
        exit(1);
    }
    server->messageLoop();
}

dword nic_read(dword nicThread, Ether::Frame* frame)
{
    MessageInfo msg;
    if (MonAPI::Message::sendReceive(&msg, nicThread, MSG_FRAME_READ))
    {
        printf("send error 1");
        return 1;
    }
    GetFrameFromSharedMemory(frame);
    return 0;
}

// caller should free() packet, after packet written
// not thread safe
dword nic_write(dword nicThread, OutPacket* packet)
{
    MessageInfo msg;
    if (MonAPI::Message::sendReceive(&msg, nicThread, MSG_FRAME_WRITE, (dword)packet))
    {
        printf("send error 1");
        return 1;
    }
	sprintf(NULL,"%d",1); // trick your linker.
    return 0;
}
