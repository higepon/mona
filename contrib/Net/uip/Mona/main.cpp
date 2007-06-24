extern "C" {
#include <uip.h>
#include <uip_arp.h>
};

#include "ServerMessageLoop.h"
#include "monadev.h"
#include "NicServer.h"
#include <monapi.h>

using namespace mones;
static NicServer* server;
void NicListenLoop();
uint32_t nic_read(uint32_t nicThread, Ether::Frame* frame);
uint32_t nic_write(uint32_t nicThread, OutPacket* packet);

uint32_t nicThread;

#define BUF ((struct uip_eth_hdr *)&uip_buf[0])

extern void MessageLoop();
extern void* test();
extern void irc();
void thread_client()
{
    MessageLoop();
//    test();
//    irc();
    return;
}

void thread_init()
{
    uint32_t id = syscall_mthread_create((uint32_t)thread_client);
// comment out by higepon
//    syscall_mthread_join(id);
}

#ifndef NULL
#define NULL (void *)0
#endif /* NULL */
int main(int argc, char* argv[])
{
    uint32_t id = syscall_mthread_create((uint32_t)NicListenLoop);
    if (MONAPI_FALSE == monapi_notify_server_start("MONITOR.BIN"))
    {
        exit(-1);
    }

// comment out by higepon
//    syscall_mthread_join(id);

    for (;;) {
        if (server != NULL && server->isStarted()) {
            break;
        }
        sleep(500);
    }
    nicThread = server->getThreadID();
    u8_t i, arptimer;

    /* Initialize the device driver. */
    monadev_init();
    thread_init();
    /* Initialize the uIP TCP/IP stack. */
    uip_init();
    /* Initialize the HTTP server. */
    httpd_init();


    arptimer = 0;

    while(1) {
        /* Let the monadev network device driver read an entire IP packet
           into the uip_buf. If it must wait for more than 0.5 seconds, it
           will return with the return value 0. If so, we know that it is
           time to call upon the uip_periodic(). Otherwise, the monadev has
           received an IP packet that is to be processed by uIP. */
        uip_len = monadev_read();
        if(uip_len == 0) {
            for(i = 0; i < UIP_CONNS; i++) {
                uip_periodic(i);
                /* If the above function invocation resulted in data that
                   should be sent out on the network, the global variable
                   uip_len is set to a value > 0. */
                if(uip_len > 0) {
                    uip_arp_out();
                    monadev_send();
                }
            }

#if UIP_UDP
            for(i = 0; i < UIP_UDP_CONNS; i++) {
                uip_udp_periodic(i);
                /* If the above function invocation resulted in data that
                   should be sent out on the network, the global variable
                   uip_len is set to a value > 0. */
                if(uip_len > 0) {
                    uip_arp_out();
                    monadev_send();
                }
            }
#endif /* UIP_UDP */

            /* Call the ARP timer function every 10 seconds. */
            if(++arptimer == 20) {
                uip_arp_timer();
                arptimer = 0;
            }

        } else {
            if(BUF->type == htons(UIP_ETHTYPE_IP)) {
                uip_arp_ipin();
                uip_input();
                /* If the above function invocation resulted in data that
                   should be sent out on the network, the global variable
                   uip_len is set to a value > 0. */
                if(uip_len > 0) {
                    uip_arp_out();
                    monadev_send();
                }
            } else if(BUF->type == htons(UIP_ETHTYPE_ARP)) {
                uip_arp_arpin();
                /* If the above function invocation resulted in data that
                   should be sent out on the network, the global variable
                   uip_len is set to a value > 0. */
                if(uip_len > 0) {
                    monadev_send();
                }
            }
        }

    }
    return 0;
}
/*-----------------------------------------------------------------------------------*/
void
uip_log(char *m)
{
    _printf("uIP log message: %s\n", m);
}
/*-----------------------------------------------------------------------------------*/

void NicListenLoop()
{
    server = new NicServer;
    if (!server->initialize())
    {
        _printf("NicServer initialize failed\n");
        exit(1);
    }
    server->messageLoop();
}

uint32_t nic_read(uint32_t nicThread, Ether::Frame* frame)
{
    MessageInfo msg;
    if (MonAPI::Message::sendReceive(&msg, nicThread, MSG_FRAME_READ))
    {
        _printf("send error 1");
        return 1;
    }
    if (1 == msg.arg2) {
        return 1;
    }

    GetFrameFromSharedMemory(frame);
    return 0;
}

// caller should free() packet, after packet written
// not thread safe
uint32_t nic_write(uint32_t nicThread, OutPacket* packet)
{
    MessageInfo msg;
    if (MonAPI::Message::sendReceive(&msg, nicThread, MSG_FRAME_WRITE, (uint32_t)packet))
    {
        _printf("send error 1");
        return 1;
    }
    return 0;
}
