#include "Net.h"
#include "IPStack.h"

using namespace mones;
using namespace MonAPI;

void IPStack::FillIPHeader(IP* ip)
{
    ip->verhead=0x45;      //version & headersize
    ip->tos=0x00;          //tos
    ip->len=bswap(60);     //totallength;
    ip->id=bswap(0xabcd);  //made from PID?
    ip->frag=bswap(0x0000);//flag
    ip->ttl=0x80;          //TTL
    ip->prot=0x01;         //ICMP
    ip->chksum=0x0;        //for calculating checksum, it should be zero.  
    ip->chksum=bswap(checksum((byte*)ip,(ip->verhead&0x0F)<<2));
}

inline word IPStack::checksum(byte *data,word size)
{
    dword sum=0;
    for(int i=0;i<=size-2;i+=2){
        sum+=bswap(*(word*)(data+i));
    }
    if(size%2==1){
        sum+=bswap(*(data+size-1)&0xFF);
    }
    return ~(((sum>>16)+sum)&0xFFFF);
}

void IPStack::dumpPacket(IP* pkt)
{
    switch( pkt->prot){
    case TYPEICMP:
        printf("ICMP:");
        break;
    case TYPETCP:
        printf("TCP:");
        break;
    case TYPEUDP:
        printf("UDP:");
        UDP* udp=pkt->UDPHeader;
        printf("R:%d L:%d LEN:%d CKSUM:%d\n",bswap(udp->srcport),
             bswap(udp->dstport),bswap(udp->len),bswap(udp->chksum));
        break;
    case TYPEIGMP:
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
