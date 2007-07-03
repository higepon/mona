/*!
    COPYRIGHT AND PERMISSION NOTICE

    Copyright (c) 2002-2007 Higepon

    All rights reserved.

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
    deal in the Software without restriction, including without limitation the
    rights to use, copy, modify, merge, publish, distribute, and/or sell copies
    of the Software, and to permit persons to whom the Software is furnished to
    do so, provided that the above copyright notice(s) and this permission
    notice appear in all copies of the Software and that both the above
    copyright notice(s) and this permission notice appear in supporting
    documentation.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF THIRD PARTY RIGHTS.
    IN NO EVENT SHALL THE COPYRIGHT HOLDER OR HOLDERS INCLUDED IN THIS NOTICE BE
    LIABLE FOR ANY CLAIM, OR ANY SPECIAL INDIRECT OR CONSEQUENTIAL DAMAGES, OR
    ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
    IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
    OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

    Except as contained in this notice, the name of a copyright holder shall not
    be used in advertising or otherwise to promote the sale, use or other
    dealings in this Software without prior written authorization of the
    copyright holder.
*/

/*!
    \file   main.cpp
    \brief  Network Server(daemon)

    This server works for remote shell.
    It listens at 5555 port and receive commands.
    And Send the command to Shell Server and receive the response, forward to client.


    If you want to add other port to listen, see the uIP manual.

    Copyright (c) 2002-2007 Higepon and the individuals listed on the ChangeLog entries.
    All rights reserved.
    License=MIT/X License

    \author  HigePon
    \version $Revision$
    \date   create:2007/06/25 update:$Date $
*/

extern "C" {
#include <uip.h>
#include <uip_arp.h>
};

#include "monadev.h"
#include "NicServer.h"
#include <monapi.h>

using namespace mones;
static NicServer* server;
void NicListenLoop();
uint32_t nic_read(uint32_t nicThread, Ether::Frame* frame);
uint32_t nic_write(uint32_t nicThread, OutPacket* packet);

#define BUF ((struct uip_eth_hdr *)&uip_buf[0])

#ifndef NULL
#define NULL (void *)0
#endif /* NULL */

// net_server は書きかけです。
// Schemeシェルでリモートシェルを実装しようとして test.rb とのデータのやりとりをやろうと思っていたけど
// まだうまくいっていません。割り込みは来ているので、uip.c あたりをさぐるかんじで。
// uipアプリケーションは ドキュメントをよんできれいにつくりましょう。
// qemuにパッチが必要だよ


int main(int argc, char* argv[])
{
//    syscall_get_io();
    if (MONAPI_FALSE == monapi_notify_server_start("MONITOR.BIN"))
    {
        exit(-1);
    }

//    syscall_mthread_create((uint32_t)NicListenLoop);

//     for (;;) {
//         if (server != NULL && server->isStarted()) {
//             break;
//         }
//         sleep(500);
//     }
    u8_t i, arptimer;

    /* Initialize the device driver. */
//    monadev_init();
    /* Initialize the uIP TCP/IP stack. */
    uip_init();

    /* Initialize the HTTP server. */
    net_server_init();

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
                    _printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
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
        printf("NicServer initialize failed\n");
        exit(1);
    }
    server->messageLoop();
}

extern std::queue<Ether::Frame*> frameQueue;

uint32_t nic_read(Ether::Frame* frame)
{
   int ret = syscall_receive_packet((uint8_t*)frame);
   if (1 == ret)
   {
//       sleep(30);
   }
   return ret;
//     if (frameQueue.empty())
//     {
//         sleep(30);
//         return 1;
//     }
//     _printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
//     Ether::Frame* f = frameQueue.front();
//     frameQueue.pop();
//     memcpy(frame, f, sizeof(Ether::Frame));
//     delete f;
//     return 0;
}

// caller should free() packet, after packet written
// not thread safe
uint32_t nic_write(OutPacket* packet)
{
    syscall_send_packet(packet->header, packet->destmac, packet->size, packet->protocol);
//    server->nic->outputFrame(packet->header, packet->destmac, packet->size, packet->protocol);
    return 0;
}
