#include <monalibc/stdio.h>
#include "Config.h"
#include "NicServer.h"
#include <monapi2/Basic/String.h>

using namespace MonAPI;
using namespace mones;

// nic interface
static NicServer* server;
void NicListenLoop();
dword nic_read(dword nicThread, Ether::Frame* frame);
dword nic_write(dword nicThread, OutPacket* packet);
bool getMacAddress(dword nicThread, byte* macAddress);

// protocol
void doArp(dword nicThread, const Config& config, Ether::Frame* frame, byte* myMacAddress);
void doIp(dword nicThread, const Config& config, Ether::Frame* frame);
void doIcmpReply(dword nicThread, const Config& config, IP::Header* fromIpHeader, Icmp::Header* fromIcmpHeader, byte* destmac);
dword getIpRoute(const Config& config, dword destIp);

int MonaMain(List<char*>* pekoe)
{
    dword ipAddress;
    dword subnetAddress;
    dword defaultGateway;

    if (pekoe->size() < 3) {
        printf("********************************************\n");
        printf("we use default address\n");
        printf("usage: arptest [ip address] [subnet address] [default gateway]\n");
        ipAddress      = Util::ipAddressToDword(192, 168, 100, 2);
        subnetAddress  = Util::ipAddressToDword(255, 255, 255, 0);
        defaultGateway = Util::ipAddressToDword(192, 168, 100, 1);
    } else {
        int a, b, c, d;
        sscanf(pekoe->get(0), "%d.%d.%d.%d", &a, &b, &c, &d);
        printf("ip address : %d.%d.%d.%d\n", a, b, c, d);
        ipAddress = Util::ipAddressToDword(a, b, c, d);

        sscanf(pekoe->get(1), "%d.%d.%d.%d", &a, &b, &c, &d);
        printf("subnet address : %d.%d.%d.%d\n", a, b, c, d);
        subnetAddress = Util::ipAddressToDword(a, b, c, d);

        sscanf(pekoe->get(2), "%d.%d.%d.%d", &a, &b, &c, &d);
        printf("default gateway : %d.%d.%d.%d\n", a, b, c, d);

        defaultGateway = Util::ipAddressToDword(a, b, c, d);
    }

    dword id = syscall_mthread_create((dword)NicListenLoop);
    syscall_mthread_join(id);
    Config config(ipAddress, subnetAddress, defaultGateway);
    Ether::Frame frame;
    byte myMacAddress[6];

    // 起動待ちいけてない
    for (;;) {
        if (server != NULL && server->isStarted()) {
            break;
        }
        sleep(500);
    }

    dword nicThread = server->getThreadID();
    getMacAddress(nicThread, myMacAddress);

    printf("\nlisten start...\n\n");
    for (;;)
    {
        nic_read(nicThread, &frame);
        word frameType = Util::get2byte((byte*)&frame, 12);

//        printf("Ether::Frame Read\n");

        switch(frameType) {
        case Ether::IP:
        {
            doIp(nicThread, config, &frame);
            break;
        }
        case Ether::ARP:
        {
            doArp(nicThread, config, &frame, myMacAddress);
            break;
        }
        default:
            printf("not my business");
            break;
        }
    }
    return 0;
}

dword getIpRoute(const Config& config, dword destIp)
{
    dword destSubnet = destIp & config.getSubnetAddress();
    dword mySubnet = config.getIpAddress() & config.getSubnetAddress();

    // 同一サブネット
    if (destSubnet == mySubnet)
    {
        return destIp;
    }

    // ルーターへ
    return config.getDefaultGateway();
}

void doIp(dword nicThread, const Config& config, Ether::Frame* frame)
{
    IP::Header* fromIpHeader = (IP::Header*)(frame->data);

    // ignore broadcast now!!
    if (fromIpHeader->dstip != config.getIpAddress())
    {
//      printf("not my business\n");
        return;
    }

    switch(fromIpHeader->prot) {
    case IP::ICMP:
    {
        Icmp::Header* fromIcmpHeader = (Icmp::Header*)(fromIpHeader->data);
        doIcmpReply(nicThread, config, fromIpHeader, fromIcmpHeader, frame->srcmac);
        break;
    }
    case IP::UDP:
    {
        UDP::Header* udp = (UDP::Header*)(fromIpHeader->data);
        printf("dest port %d udp->length  %d\n", Util::swapShort(udp->dstport), Util::swapShort(udp->len));
        if (Util::swapShort(udp->dstport) == 8888)
        {
            char buf[1024];
            memcpy(buf, udp->data, Util::swapShort(udp->len));
            buf[udp->len] = '\0';
            printf("message : %s\n", buf);
        }

        break;
    }
    default:
        printf("sorry IP protocol %d not supported yet\n", fromIpHeader->prot);
        break;
    }
}

void doIcmpReply(dword nicThread, const Config& config, IP::Header* fromIpHeader, Icmp::Header* fromIcmpHeader, byte* destmac)
{
    OutPacket* out = new OutPacket;
    IP::Header* ipHeader = (IP::Header*)(out->header);
    Icmp::Header* icmpHeader = (Icmp::Header*)(ipHeader->data);
    int size = Util::swapShort(fromIpHeader->len) - sizeof(IP::Header);
    printf("ICMP Echo Request comes  ");
    // ICMP
    memcpy(icmpHeader->data, fromIcmpHeader->data, 32); // copy id, sequence from source icmp
    icmpHeader->type   = 0;
    icmpHeader->code   = 0;
    icmpHeader->chksum = 0;
    icmpHeader->chksum = Util::calcCheckSum((dword*)icmpHeader, size);

    // IP
    ipHeader->verhead = (4 << 4) | (sizeof(IP::Header) / 4);
    ipHeader->tos    = 0;
    ipHeader->id     = fromIpHeader->id + 1;
    ipHeader->ttl    = 255;
    ipHeader->prot   = IP::ICMP;
    ipHeader->srcip  = config.getIpAddress();
    ipHeader->dstip  = fromIpHeader->srcip;
    ipHeader->chksum = 0;
    ipHeader->len    = Util::swapShort(sizeof(IP::Header) + size);
    ipHeader->frag   = 0;
    ipHeader->chksum = Util::calcCheckSum((dword*)ipHeader, sizeof(IP::Header));

    // OutPacket
    memcpy(out->destmac, destmac, 6);
    out->protocol = 0x800;
    out->size = sizeof(IP::Header) + size;
    nic_write(nicThread, out);
    printf("send ICMP Echo Reply\n");
    delete out;
}

void doArp(dword nicThread, const Config& config, Ether::Frame* frame, byte* myMacAddress)
{
    Arp::Header* rheader = (Arp::Header*)(frame->data);

    if (rheader->dstIp != config.getIpAddress()) return;;

    switch (Util::swapShort(rheader->opeCode))
    {
    case Arp::OPE_CODE_ARP_REQ:
    {
        OutPacket packet;
        printf("ARP Request comes  ");
        Arp::Header temp;
        temp.hardType =Util::swapShort(Arp::HARD_TYPE_ETHER);
        temp.protType = Util::swapShort(Arp::PROTCOL_TYPE_IP);
        temp.hardAddrLen = 6;
        temp.protAddrLen = 4;
        memcpy(temp.srcMac, myMacAddress, 6);
        temp.opeCode = Util::swapShort(Arp::OPE_CODE_ARP_REP);
        temp.srcIp = config.getIpAddress();
        temp.dstIp = rheader->srcIp;
        memcpy(temp.dstMac, rheader->srcMac, 6);
        memcpy(packet.header, &temp, sizeof(temp));
        memcpy(packet.destmac, rheader->srcMac, 6);
        packet.size = sizeof(temp);
        packet.protocol = Ether::ARP;
        printf("send ARP Reply\n");
        nic_write(nicThread, &packet);
        break;
    }
    default:
        printf("arp operation %x \n", Util::swapShort(rheader->opeCode));
        break;
    }
}

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
    return 0;
}

bool getMacAddress(dword nicThread, byte* macAddress)
{
    MessageInfo msg;
    if (MonAPI::Message::sendReceive(&msg, nicThread, MSG_GET_MAC_ADDRESS))
    {
        printf("send error 2");
        return false;
    }
    memcpy(macAddress, msg.str, 6);
    return true;
}
