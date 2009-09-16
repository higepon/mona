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
#include "Util.h"
#include "DHCPClient.h"

extern "C" {
#include <uip.h>
#include <uip_arp.h>
}

#define UIP_ASSERT(condition) {if (!(condition)) {_printf("ASSERT failure %s:%d: %s\n", __FILE__, __LINE__, #condition);exit(-1);}}


uintptr_t ContigousPhysicalMemory::startAddress = 0x9E000000;

static VirtioNet* virtioNet = NULL;;
extern "C" void uip_loop();

extern "C" unsigned int dev_read()
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

extern "C" void dev_send()
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

#ifdef USE_QEMU_USER_NETWORK
    _printf("\n\n[uIP Web Server] Access: http://localhost:8080 from your HOST of qemu\n\n");
#endif
    virtioNet = new VirtioNet();
    const int numberOfReadBufferes = 5;
    enum VirtioNet::DeviceState state = virtioNet->probe(numberOfReadBufferes);
    if (state != VirtioNet::DEVICE_FOUND) {
        _printf("[virtio] virtio-net device not found\n");
        exit(-1);
    }
// qemu -net user mode:
//   we send DHCP request to QEMU and get an ip address.
#ifdef USE_QEMU_USER_NETWORK
    DHCPClient dhcp(virtioNet, virtioNet->macAddress());;
    uint32_t hostAddress = 0;
    uint32_t gatewayAddress = 0;
    if (!dhcp.request(hostAddress, gatewayAddress)) {
        _printf("[uIP] DHCP server not found. exit server\n");
        exit(-1);
    }
#endif


        /* Initialize the uIP TCP/IP stack. */
        uip_init();

#ifdef USE_QEMU_USER_NETWORK
        uint16_t addr[2];
        Util::ipAddressToU16Array(addr, hostAddress);

        // QEMU will offer 10.0.2.x (x >= 15).
        UIP_ASSERT(addr[0] == 0x000a);
        UIP_ASSERT((addr[1] & 0xff) == 2);
        UIP_ASSERT((addr[1] >> 8) >= 15);
        uip_sethostaddr(addr);
        uip_setdraddr(Util::ipAddressToU16Array(addr, gatewayAddress));
        uip_setnetmask(Util::ipAddressToU16Array(addr, 0x00ffffff /* 255.255.255.0 */));
#endif

   uip_loop();
    return 0;
}
