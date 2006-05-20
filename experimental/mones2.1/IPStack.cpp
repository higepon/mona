#include "IPStack.h"

using namespace mones;
using namespace MonAPI;

void IPStack::FillIPHeader(IP* ip)
{
    ip->verhead=0x45;      //version & headersize
    ip->tos=0x00;          //tos
    ip->len=bswap(60);     //totallength;
    ip->id=bswap(0xCFEE);  //made from PID?
    ip->frag=bswap(0x0000);//flag
    ip->ttl=0x80;          //TTL
    ip->prot=0x01;         //ICMP
    ip->chksum=0xabb0;     //CKSUM
}

void IPStack::dumpPacket(IP* pkt)
{
    switch( pkt->prot){
    case IP::TYPEICMP:
        printf("ICMP:");
        break;
    case IP::TYPETCP:
        printf("TCP:");
        break;
    case IP::TYPEUDP:
        printf("UDP:");
        UDP* udp=pkt->UDPHeader;
        printf("R:%d L:%d LEN:%d CKSUM:%d\n",bswap(udp->srcport),
             bswap(udp->dstport),bswap(udp->len),bswap(udp->chksum));
        break;
    case IP::TYPEIGMP:
        printf("IGMP:");
        break;
    default:
        printf("orz.");
    }
    printf("src:");
    for(int j=0;j<4;j++)
        printf("%d.",*(((byte*)&(pkt->srcip))+j));
    printf("dst:");
    for(int j=0;j<4;j++)
        printf("%d.",*(((byte*)&(pkt->dstip))+j));
    printf("\n");
}

void IPStack::FillICMPHeader(ICMP* icmp,ICMP* src)
{

}