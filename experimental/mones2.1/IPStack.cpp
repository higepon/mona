//$Id$
#include <pci/Pci.h>
#include "Net.h"
#include "NE2000.h"
#include "MonAMDpcn.h"
#include "LoopBack.h"
#include "IPStack.h"

using namespace mones;
using namespace MonAPI;

IPStack::IPStack()
{
    PciInf pciinfo;
    Pci* pcilib = new Pci();
    printf("\nIP address is defined in IPstack constructor. Rewrite it as your environment.\n");
    pcilib->CheckPciExist(MonAMDpcn::VENDORID,MonAMDpcn::DEVICEID,&pciinfo);
    if( pciinfo.Exist== 0){
        // 0x04 is PCI_COMMAND
        dword val=pcilib->ReadConfig(0, pciinfo.DeviceNo, 0, 0x04,2);
        pcilib->WriteConfig(0,pciinfo.DeviceNo,0,0x04,2,val|0x4);
        nic=new MonAMDpcn();
        nic->setIP(192,168,0,5);  //Vmware. 
        nic->setIRQ(pciinfo.IrqLine);
        nic->setIOBase(pciinfo.BaseAd);
        if( nic->init() == 0 ){
            delete pcilib;
            return;
        }
        delete nic;
        nic=NULL;
    }

    pcilib->CheckPciExist(NE2000::VENDORID,NE2000::DEVICEID,&pciinfo);
    if( pciinfo.Exist == 0){
        nic = new NE2000();
        nic->setIP(192,166,0,5); //QEMU.
        nic->setIRQ(pciinfo.IrqLine);
        nic->setIOBase(pciinfo.BaseAd);
        if( nic->init() == 0 ){
            delete pcilib;
            return;
        }
        delete nic;
        nic=NULL;
    }
    delete pcilib;

    nic = new LoopBack();
    nic->setIP(LOOPBACKIP&0xFF,0x00,0x00,LOOPBACKIP>>24);
    nic->setIRQ(0);
    nic->setIOBase(0);
    if( nic->init() == 0 ){
        return;
    }
    delete nic;

    nic = new NE2000();
    nic->setIP(172,16,110,4);//Bochs.
    nic->setIRQ(3);
    nic->setIOBase(0x240);
    if( nic->init() != 0 ){
        delete nic;
        nic=NULL;
    }
}

IPStack::~IPStack()
{
    if( (nic != NULL) && (nic->getIRQ() != 0 ) ){
        syscall_remove_irq_receiver(this->nic->getIRQ());
        delete nic;
    }
}

bool IPStack::initialize()
{
    if( nic != NULL ){
        if( nic->getIRQ()!=0 ) // with mask Interrrupt by higepon
            syscall_set_irq_receiver(this->nic->getIRQ(), SYS_MASK_INTERRUPT); 
         this->nic->enableNetwork();
        return true;
    }else return false;
}

void IPStack::dumpPacket(Ether* frame)
{
    IP* pkt=frame->IPHeader;
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

word IPStack::checksum(byte *data,word size)
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

//////////////////////////////////////////////////////////////////////////////////////////
int IPStack::Send(byte* data,int size, CID* id)
{              
    Ether* frame = nic->CreateFrm(id->remoteip);
    //ether header has been already filled.
    if( frame != NULL){
        int headersize=0;
        switch(id->protocol)
        {
        case TYPEICMP:
            ICMP* icmp=frame->IPHeader->ICMPHeader;
            icmp->type=ECHOREQUEST;
            icmp->code=0x00;
            icmp->chksum=0x0000;
            icmp->idnum=0x0000;
            icmp->seqnum=0x0000;
            memcpy(frame->IPHeader->UDPHeader->data,data,size);
            icmp->chksum=bswap(checksum((byte*)icmp,size+sizeof(ICMP)));
            headersize=sizeof(ICMP)+sizeof(IP); //windows 8+20+32
            break;
        case TYPEUDP:
            IP* ip=frame->IPHeader; //for psedo header
            ip->ttl =0x00;
            ip->prot=0x11;
            ip->chksum=bswap(size+sizeof(UDP));
            //ip->srcip=nic->getIP(); already filled.
            //ip->dstip=id->remoteip; already filled.
            UDP* udp=frame->IPHeader->UDPHeader;
            udp->srcport=bswap(id->localport);
            udp->dstport=bswap(id->remoteport);
            udp->len=bswap(size+sizeof(UDP));
            udp->chksum=0x0000;
            memcpy(frame->IPHeader->UDPHeader->data,data,size);
            udp->chksum=bswap(checksum((byte*)&(ip->ttl),size+sizeof(UDP)+12));
            headersize=sizeof(UDP)+sizeof(IP);
        case TYPETCP:
            break;
        default:
            printf("orz\n");
        }
        FillIPHeader(frame,size+headersize,id->protocol);
        nic->SendFrm(frame);
    }
    return 0;
}

void IPStack::ICMPreply(Ether* frame)
{
    Ether* rframe = nic->CreateFrm(frame->IPHeader->srcip);
    //ether header has been already filled.
    if( rframe != NULL){
        ICMP* icmp=rframe->IPHeader->ICMPHeader;
        icmp->type=ECHOREPLY;
        icmp->code=0x00;
        icmp->chksum=0x0000;
        icmp->idnum=frame->IPHeader->ICMPHeader->idnum;
        icmp->seqnum=frame->IPHeader->ICMPHeader->seqnum;
        memcpy(icmp->data,frame->IPHeader->ICMPHeader->data,bswap(frame->IPHeader->len)-sizeof(IP)-sizeof(ICMP));
        icmp->chksum=bswap(checksum((byte*)icmp,bswap(frame->IPHeader->len)-sizeof(IP)));
        printf("replying.\n");
        FillIPHeader(rframe,bswap(frame->IPHeader->len),TYPEICMP);
        printf("asdf\n");
        nic->SendFrm(rframe);
    }
}

void IPStack::FillIPHeader(Ether* frame,word length,byte protocol)
{
    IP* ip=frame->IPHeader;
    ip->verhead=0x45;       //version & headersize
    ip->tos=0x00;           //tos
    ip->len=bswap(length);  //totallength PING size windows 60 solaris 84
    ip->id=bswap(0xabcd);   //made from PID?
    ip->frag=bswap(0x0000); //flag
    ip->ttl=0x80;           //TTL
    ip->prot=protocol;      
    ip->chksum=0x0;         //for calculating checksum, it should be zero.  
    ip->chksum=bswap(checksum((byte*)ip,(ip->verhead&0x0F)<<2));
}

void IPStack::PeriodicUpdate()
{

}

