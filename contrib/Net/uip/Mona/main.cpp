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

#include "dhcp.h"

class IP {
public:
    enum {
        ICMP = 0x01,
        UDP = 0x11,
    };
    typedef struct{
        uint32_t  hl:4,              /* header length */
                 version:4;               /* version */
        uint8_t  tos;               /* type of service */
        uint16_t len;               /* total length */
        uint16_t id;                /* identification */
        uint16_t off;               /* fragment offset field */
        uint8_t  ttl;               /* time to live */
        uint8_t  prot;                 /* protocol */
        uint16_t chksum;               /* checksum */
        uint32_t srcip;
        uint32_t dstip;
        char     data[0];
    } Header;
};
class UDP {
public:
 typedef struct{
    uint16_t srcport;
    uint16_t dstport;
    short len;
    uint16_t chksum;
    char   data[0];
} Header;
};

class Util
{
public:
    inline static uint16_t* ipAddressToU16Array(uint16_t* buf, uint32_t address)
    {
        buf[0] = address & 0xffff;
        buf[1] = address >> 16;
        return buf;
    }

    inline static uint32_t swapLong(uint32_t value)
    {
        return (value>>24)+((value>>8)&0xff00)+((value<<8)&0xff0000)+(value<<24);
    }

    inline static uint16_t swapShort(uint16_t value)
    {
        return (value>>8)+(value<<8);
    }

    inline static uint16_t get2uint8_t(uint8_t *buf, uint32_t offset)
    {
        uint16_t a;
        a = buf[offset++] << 8;
        return a | buf[offset];
    }

    inline static uint32_t ipAddressToDuint16_t(uint8_t a, uint8_t b, uint8_t c, uint8_t d)
    {
        return (d << 24) | (c << 16) | (b << 8) | a;
    }

    inline static MonAPI::CString ipAddressToCString(uint32_t a)
    {
        char buf[64];
        sprintf(buf, "%d.%d.%d.%d", a & 0xff, (a >> 8) & 0xff, (a >> 16) & 0xff, (a >> 24) & 0xff);
        return MonAPI::CString(buf);
    }

    inline static uint16_t calcCheckSum(uint32_t *data,int size)
    {
        union{
            unsigned long long u64;
            uint32_t            u32[2];
            uint16_t             u16[4];
        }sum;

        uint32_t tmp;


        sum.u64=0;
        for(;(uint32_t)size>=sizeof(uint32_t);size-=sizeof(uint32_t))
            sum.u64+=*data++;
        if(size>0)sum.u64+=*data&((1<<(size*8))-1);

        tmp=sum.u32[1];
        sum.u32[1]=0;
        sum.u64+=tmp;
        tmp=sum.u32[1];
        sum.u32[1]=0;
        sum.u32[0]+=tmp;

        tmp=sum.u16[1];
        sum.u16[1]=0;
        sum.u32[0]+=tmp;
        tmp=sum.u16[1];
        sum.u16[1]=0;
        sum.u16[0]+=tmp;

        return ~sum.u16[0];
    }

};

#define UIP_ASSERT(condition) {if (!(condition)) {_printf("ASSERT failure %s:%d: %s\n", __FILE__, __LINE__, #condition);exit(-1);}}

class DHCPClient {
private:
    VirtioNet* virtioNet_;
    dhcp_packet packet_;
    uint8_t srcmac_[6];
public:
    DHCPClient(VirtioNet* virtioNet, uint8_t* srcmac) : virtioNet_(virtioNet)
    {
        for (int i = 0; i < 6; i++) {
            srcmac_[i] = srcmac[i];
        }
    }

    bool request(uint32_t& hostAddress, uint32_t& gatewayAddress)
    {
        Ether::Frame frame;
        frame.type = Util::swapShort(Ether::IP);
        // broad cast
        for (int i = 0; i < 6; i++) {
            frame.dstmac[i] = 0xff;
            frame.srcmac[i] = srcmac_[i];
        }
        const int SIZE_OF_DHCP_PACKET = sizeof(dhcp_packet) - DHCP_OPTION_LEN;

        // IP
        IP::Header* ip = (IP::Header*)frame.data;
        ip->version = 4;
        ip->hl = 5;
        ip->prot = IP::UDP;
        ip->ttl = 64;
        ip->off = 0x0040;
        ip->srcip = 0x00000000;
        ip->dstip = 0xffffffff;
        ip->len = Util::swapShort(sizeof(IP::Header) + sizeof(UDP::Header) + SIZE_OF_DHCP_PACKET);
        ip->chksum = 0;
        ip->chksum = Util::calcCheckSum((uint32_t*)ip, sizeof(IP::Header));

        // UDP
        UDP::Header* udp = (UDP::Header*)ip->data;
        udp->srcport = Util::swapShort(0x44);
        udp->dstport = Util::swapShort(0x43);
        udp->len =  Util::swapShort(sizeof(UDP::Header) + SIZE_OF_DHCP_PACKET);
        udp->chksum = 0x00; // 0 means none
//        udp->chksum = Util::calcCheckSum((uint32_t*)udp, sizeof(UDP::Header));

        dhcp_packet* dhcp = (dhcp_packet*)udp->data;
        dhcp->op = BOOTREQUEST;
        dhcp->htype = 0x01;
        dhcp->hlen = 0x06;
        dhcp->hops = 0x00;
        dhcp->xid = 0x12345678;
        dhcp->secs = 0;
        dhcp->flags = 0;
        dhcp->ciaddr = 0;
        dhcp->yiaddr = 0;
        dhcp->siaddr = 0;
        dhcp->giaddr = 0;
        memcpy(dhcp->chaddr, srcmac_, 6);
        memset(dhcp->chaddr + 6, 0x00, 10);
        memset(dhcp->sname, 0x00, DHCP_SNAME_LEN);
        memset(dhcp->file, 0x00, DHCP_FILE_LEN);

        dhcp->options[0] = 99;
        dhcp->options[1] = 130;
        dhcp->options[2] = 83;
        dhcp->options[3] = 99;
        dhcp->options[4] = 53; //RFC2132_MSG_TYPE
        dhcp->options[5] = 1;  // len
        dhcp->options[6] = DHCPDISCOVER;
        _printf("dhcp size=%d\n", 18 + sizeof(IP::Header) + sizeof(UDP::Header) + SIZE_OF_DHCP_PACKET);
        virtioNet_->send(&frame, 18 + sizeof(IP::Header) + sizeof(UDP::Header) + sizeof(dhcp_packet));

        memset(&frame, 0, sizeof(Ether::Frame));
        unsigned int len;
        for (;;) {

            if (virtioNet_->receive(&frame, &len, 1000)) {
                if (frame.type == 0x08) {
                    _printf("IP\n");
                    IP::Header* ip = (IP::Header*)frame.data;   
                    if (ip->prot == IP::UDP) {
                        _printf("UDP\n");
                        UDP::Header* udp = (UDP::Header*)ip->data;
                        if (udp->srcport == Util::swapShort(0x43)) {
                            dhcp_packet* dhcp = (dhcp_packet*)udp->data;
                            if (dhcp->op == 0x02) {
                                _printf("UDP DHCP Offer\n");
                                _printf("addr=%sn", (const char*)Util::ipAddressToCString(dhcp->yiaddr));
                                 uint16_t addr[2] ;
                                 hostAddress = dhcp->yiaddr;
//                                 uip_init();
//                                 addr[0] = dhcp->yiaddr & 0xffff;
//                                 addr[1] = dhcp->yiaddr >> 16;

//                                 // QEMU will offer 10.0.2.x (x >= 15).
//                                 UIP_ASSERT(addr[0] == 0x000a);
//                                 UIP_ASSERT((addr[1] & 0xff) == 2);
//                                 UIP_ASSERT((addr[1] >> 8) >= 15);

                                 gatewayAddress = dhcp->siaddr;
//                                 uip_sethostaddr(addr);
//                                 addr[0] = dhcp->siaddr & 0xffff;
//                                 addr[1] = dhcp->siaddr >> 16;
//                                 uip_setdraddr(addr);
//                                 addr[0] = 0x00ffffff & 0xffff;
//                                 addr[1] = 0x00ffffff >> 16;
//                                 uip_setnetmask(addr);
//                                  uip_gethostaddr(addr);
//                                  _printf("%x %x\n", addr[0], addr[1]);
                                _printf("siaddr=%sn", (const char*)Util::ipAddressToCString(dhcp->siaddr));
                                _printf("giaddr=%sn", (const char*)Util::ipAddressToCString(dhcp->giaddr));
                                for (int i = 0; i < 6; i++) {
                                    frame.dstmac[i] = 0xff;
                                    frame.srcmac[i] = srcmac_[i];
                                }
                                IP::Header* ip = (IP::Header*)frame.data;
                                ip->srcip = 0x00000000;
                                ip->dstip = 0xffffffff;
                                ip->chksum = 0;
                                ip->chksum = Util::calcCheckSum((uint32_t*)ip, sizeof(IP::Header));
                                UDP::Header* udp = (UDP::Header*)ip->data;
                                udp->srcport = Util::swapShort(0x44);
                                udp->dstport = Util::swapShort(0x43);
                                udp->len =  Util::swapShort(sizeof(UDP::Header) + SIZE_OF_DHCP_PACKET);
                                udp->chksum = 0;
                                dhcp_packet* dhcp = (dhcp_packet*)udp->data;
                                dhcp->op = BOOTREQUEST;
        dhcp->options[0] = 99;
        dhcp->options[1] = 130;
        dhcp->options[2] = 83;
        dhcp->options[3] = 99;
        dhcp->options[4] = 53; //RFC2132_MSG_TYPE
        dhcp->options[5] = 1;  // len
        dhcp->options[6] = DHCPREQUEST;

                                virtioNet_->send(&frame, 18 + sizeof(IP::Header) + sizeof(UDP::Header) + sizeof(dhcp_packet));
                                break;
                            } else {
                                _printf("not dhcp offer\n");
                            }
                        } else {
                            _printf("UDP NOT DHCP\n");
                        }
                    } else {
                        _printf("NOT UDP%d\n", ip->prot);
                    }
                } else {
                    _printf("NOT IP %x \n", frame.type);
                }
            } else {
                _printf("timeout\n");
                continue;
            }

        }

        return true;
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

// qemu -net user mode:
//   we send DHCP request to QEMU and get an ip address.
#ifdef USE_QEMU_USER_NETWORK
    DHCPClient dhcp(&virtioNet, virtioNet.macAddress());;
    uint32_t hostAddress = 0;
    uint32_t gatewayAddress = 0;
    if (!dhcp.request(hostAddress, gatewayAddress)) {
        _printf("[uIP] DHCP server not found. exit server\n");
        exit(-1);
    }
#endif

    {
        u8_t i, arptimer;

        /* Initialize the device driver. */
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
                _printf("there");
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
                    _printf("[here]\n");
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
