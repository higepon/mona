/*
 * main.cpp - Entry point of uIP(Network protocol stack) for Mona
 *
 *   Copyright (c) 2009  Higepon(Taro Minowa)  <higepon@users.sourceforge.jp>
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 *   TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  $Id$
 */
#include <monapi.h>
#include <monalibc.h>
#include "Ether.h"
#include "ContigousPhysicalMemory.h"
#include "VirtioNet.h"



extern "C" {
  #include <uip.h>
  #include <uip_arp.h>
  void uip_log(char *m)
  {
      printf("uIP log message: %s\n", m);
  }
};

uintptr_t ContigousPhysicalMemory::startAddress = 0x9E000000;

static unsigned int dev_read(VirtioNet* virtioNet)
{
    Ether::Frame frame;
    unsigned int len;
    // uIP requires, timeout with 500 msec.
    if (!virtioNet->receive(&frame, &len, 500)) {
        return 0;
    } else {
        memcpy(uip_buf, &frame, len);
//        _printf("[uIP] packet comes\n");
        return len;
    }
}

static void dev_send(VirtioNet* virtioNet)
{
    char tmpbuf[UIP_BUFSIZE];
    int i;
    for(i = 0; i < 40 + UIP_LLH_LEN; i++) {
        tmpbuf[i] = uip_buf[i];
    }

    for(; i < uip_len; i++) {
        tmpbuf[i] = uip_appdata[i - 40 - UIP_LLH_LEN];
    }

    Ether::Frame frame;
    memcpy(&frame, tmpbuf, uip_len);
    virtioNet->send(&frame, uip_len);
    return;
}

int main(int argc, char* argv[])
{
    // OK, I've started up.
    if (MONAPI_FALSE == monapi_notify_server_start("MONITOR.BIN"))
    {
        _printf("[uIP]: MONITOR server doesn't exist\n");
        exit(-1);
    }

    VirtioNet virtioNet;
    const int numberOfReadBufferes = 5;
    enum VirtioNet::DeviceState state = virtioNet.probe(numberOfReadBufferes);
    if (state != VirtioNet::DEVICE_FOUND) {
        _printf("[virtio] virtio-net device not found\n");
        exit(-1);
    }

    {
        u8_t i, arptimer;

        /* Initialize the device driver. */
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
            uip_len = dev_read(&virtioNet);
            if(uip_len == 0) {
                for(i = 0; i < UIP_CONNS; i++) {
                    uip_periodic(i);
                    /* If the above function invocation resulted in data that
                       should be sent out on the network, the global variable
                       uip_len is set to a value > 0. */
                    if(uip_len > 0) {
                        uip_arp_out();
                        dev_send(&virtioNet);
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
                        dev_send(virtioNet);
                    }
                }
#endif /* UIP_UDP */

                /* Call the ARP timer function every 10 seconds. */
                if(++arptimer == 20) {
                    uip_arp_timer();
                    arptimer = 0;
                }

            } else {
#define BUF ((struct uip_eth_hdr *)&uip_buf[0])
                if(BUF->type == htons(UIP_ETHTYPE_IP)) {
                    uip_arp_ipin();
                    uip_input();
                    /* If the above function invocation resulted in data that
                       should be sent out on the network, the global variable
                       uip_len is set to a value > 0. */
                    if(uip_len > 0) {
                        uip_arp_out();
                        dev_send(&virtioNet);
                    }
                } else if(BUF->type == htons(UIP_ETHTYPE_ARP)) {
                    uip_arp_arpin();
                    /* If the above function invocation resulted in data that
                       should be sent out on the network, the global variable
                       uip_len is set to a value > 0. */
                    if(uip_len > 0) {
                        dev_send(&virtioNet);

                    }
                }
            }

        }
        return 0;
    }
}
