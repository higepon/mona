/*
 * DHCPClient.h - 
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
 *  $Id: DHCPClient.h 261 2008-07-25 06:16:44Z higepon $
 */

#ifndef _DHCPCLIENT_
#define _DHCPCLIENT_

#include "dhcp.h"
#include "IP.h"
#include "UDP.h"

class DHCPClient {
private:
    VirtioNet* virtioNet_;
    dhcp_packet packet_;
    uint8_t srcmac_[6];
    int SIZE_OF_DHCP_PACKET;

    void sendDiscovery()
    {
        Ether::Frame frame;
        frame.type = Util::swapShort(Ether::IP);
        // broad cast
        for (int i = 0; i < 6; i++) {
            frame.dstmac[i] = 0xff;
            frame.srcmac[i] = srcmac_[i];
        }
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
        udp->chksum = 0x00; // 0 means "don't check checksum

        // DHCP
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
        virtioNet_->send(&frame, 18 + sizeof(IP::Header) + sizeof(UDP::Header) + sizeof(dhcp_packet));
    }

    bool receiveOffer(Ether::Frame* frame, uint32_t& hostAddress, uint32_t& gatewayAddress, int sec)
    {
        unsigned int len = 0;
        for (int i = 0; i < sec; i++) {
            if (virtioNet_->receive(frame, &len, 1000)) {
                if (!frame->type == 0x08) {
                    continue;
                }
                IP::Header* ip = (IP::Header*)frame->data;

                if (ip->prot != IP::UDP) {
                    continue;
                }
                UDP::Header* udp = (UDP::Header*)ip->data;

                if (udp->srcport != Util::swapShort(0x43)) {
                    continue;
                }
                dhcp_packet* dhcp = (dhcp_packet*)udp->data;
                if (dhcp->op != 0x02) {
                    continue;
                }
                uint16_t addr[2] ;
                hostAddress = dhcp->yiaddr;
                gatewayAddress = dhcp->siaddr;
                for (int i = 0; i < 6; i++) {
                    frame->dstmac[i] = 0xff;
                    frame->srcmac[i] = srcmac_[i];
                }
                return true;
            }
        }
        return false;
    }

    void sendRequest(Ether::Frame* frame)
    {
        IP::Header* ip = (IP::Header*)frame->data;
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
        virtioNet_->send(frame, 18 + sizeof(IP::Header) + sizeof(UDP::Header) + sizeof(dhcp_packet));
    }
public:
    DHCPClient(VirtioNet* virtioNet, uint8_t* srcmac) : virtioNet_(virtioNet), SIZE_OF_DHCP_PACKET(sizeof(dhcp_packet) - DHCP_OPTION_LEN + 7)
    {
        for (int i = 0; i < 6; i++) {
            srcmac_[i] = srcmac[i];
        }
    }

    bool request(uint32_t& hostAddress, uint32_t& gatewayAddress)
    {
        Ether::Frame frame;
        sendDiscovery();
        unsigned int len;

        if (!receiveOffer(&frame, hostAddress, gatewayAddress, 10 /* sec */)) {
            return false;
        }
        sendRequest(&frame);
        return true;
    }
};

#endif // _DHCPCLIENT_
