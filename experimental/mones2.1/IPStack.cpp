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

////////////////////////////////////////////////////
//reply icmp echo request,dispose unknown packet and
//get destination info. (recursive)
//return false only if buffer is empty.
////////////////////////////////////////////////////
bool IPStack::GetDestination(int n,ConnectionInfo* cinfo)
{ 
    Ether* frame = nic->RecvFrm(n);
    if( frame == NULL )
        return false;
    cinfo->protocol=frame->IPHeader->prot;
    cinfo->remoteip=frame->IPHeader->srcip;    
    cinfo->remoteport=0;
    cinfo->localport=0;
    switch (cinfo->protocol)
    {
    case TYPEICMP:
        ICMP* icmp=frame->IPHeader->ICMPHeader;
        if( icmp->type==ECHOREQUEST){
            ICMPCoInfo* info=(ICMPCoInfo*)cinfo;
            info->type=ECHOREPLY;
            info->idnum=bswap(icmp->idnum);
            info->seqnum=bswap(icmp->seqnum);
            Send(icmp->data,bswap(frame->IPHeader->len)-sizeof(IP)-sizeof(ICMP),cinfo);      
            Dispose(n);
            GetDestination(n,cinfo);//see next packet;
        }else if( icmp->type != ECHOREPLY){
            Dispose(n);
            GetDestination(n,cinfo);//see next packet;
        }
        //ECHOREPLY may be passed to client.
        break;
    case TYPEUDP: 
        cinfo->localport=bswap(frame->IPHeader->UDPHeader->dstport);
        cinfo->remoteport=bswap(frame->IPHeader->UDPHeader->srcport);
        if( UDPWellKnownSVCreply(frame) ){
            Dispose(n);
            GetDestination(n,cinfo);
        }    
        break;
    case TYPETCP:     
        cinfo->localport=bswap(frame->IPHeader->TCPHeader->dstport);
        cinfo->remoteport=bswap(frame->IPHeader->TCPHeader->srcport);
        if( HandShakePASV(frame)){
            Dispose(n);
            GetDestination(n,cinfo);
        }
        break;
    default:
        printf("unknown Packet type");
        Dispose(n);
        GetDestination(n,cinfo);
    }
    return true;
}

int IPStack::Recv(byte** data, int n)
{
    Ether* frame=nic->RecvFrm(n);
    if( frame == NULL){
        data=NULL;
        return 0;
    }
    int size=0;
    switch(frame->IPHeader->prot)
    {
    case TYPEICMP:
        size=bswap(frame->IPHeader->len)-sizeof(IP)-sizeof(ICMP);
        *data=frame->IPHeader->ICMPHeader->data;
        break;
    case TYPEUDP:        
        size=bswap(frame->IPHeader->len)-sizeof(IP)-sizeof(UDP);
        *data=frame->IPHeader->UDPHeader->data;
        break;
    case TYPETCP:
        size=bswap(frame->IPHeader->len)-sizeof(IP)-sizeof(TCP);
        *data=frame->IPHeader->TCPHeader->data;
        break;
    default:
        printf("orz\n");
        *data=NULL;
    }
    return size;
}

int IPStack::Send(byte* data,int size, ConnectionInfo* cinfo)
{              
    Ether* frame = nic->NewFrame(cinfo->remoteip);
    //ether header has been already filled.
    if( frame != NULL){
        int headersize=0;
        switch(cinfo->protocol)
        {
        case TYPEICMP:
            ICMP* icmp=frame->IPHeader->ICMPHeader;
            ICMPCoInfo* info= (ICMPCoInfo*)cinfo;
            icmp->type=info->type;
            icmp->code=0x00;
            icmp->chksum=0x0000;
            icmp->idnum=bswap(info->idnum);
            icmp->seqnum=bswap(info->seqnum);
            memcpy(icmp->data,data,size);
            icmp->chksum=bswap(checksum((byte*)icmp,size+sizeof(ICMP)));
            headersize=sizeof(ICMP)+sizeof(IP);
            break;
        case TYPEUDP:
            IP* ip=frame->IPHeader; //for psedo header
            ip->ttl =0x00;
            ip->prot=TYPEUDP;
            ip->chksum=bswap(size+sizeof(UDP));
            UDP* udp=frame->IPHeader->UDPHeader;
            udp->srcport=bswap(cinfo->localport);
            udp->dstport=bswap(cinfo->remoteport);
            udp->len=bswap(size+sizeof(UDP));
            udp->chksum=0x0000;
            memcpy(frame->IPHeader->UDPHeader->data,data,size);
            udp->chksum=bswap(checksum((byte*)&(ip->ttl),size+sizeof(UDP)+12));
            headersize=sizeof(UDP)+sizeof(IP);
        case TYPETCP:
            //HandShakeACTIV();
            break;
        default:
            printf("orz\n");
        }
        CreateIPHeader(frame,size+headersize,cinfo->protocol);
        nic->SendFrm(frame);
    }
    return 0;
}

//   CLOSED       -> LISTENING @readAPI    ==
//   ESTABLISHED  -> FIN_WAIT1 @closeAPI   send FIN
//   ~ESTABLISHED -> CLOSED    @timerAPI   ==
bool IPStack::HandShakeACTV(Ether* frame)
{
    printf("ACTV\n");
    //CLOSED     -> SYN_SENT               send SYN
    //SYN_RCVD   -> SYN_RCVD               send SYN+ACK
    //SYN_SENT   -> ESTABLISHED            send ACK
    //CLOSE_WAIT -> LAST_ACK               send FIN
    //TIME_WAIT  -> TIME_WAIT              send ACK
    //ESTABLISHED->ESTABLISHED             send -
    return false;
}

bool IPStack::HandShakePASV(Ether* frame)
{
    printf("PASV\n");
    //CLOSED     -> CLOSED                 ==
    //LISTENING  -> SYN_RCVD               rcv SYN  (send SYN+ACK)
    //SYN_SENT   -> SYN_SENT               rcv SYN+ACK (send ACK)
    //SYN_RCVD   -> ESTABLISHED            rcv ACK
    //ESTABLISHED-> CLOSE_WAIT             rcv FIN
    //FIN_WAIT2  -> TIME_WAIT              rcv ACK
    //LAST_ACK   -> CLOSED                 rcv ACK
    //ESTABLISHED-> ESTABLISHED            rcv -
    return false;
}

bool IPStack::UDPWellKnownSVCreply(Ether* frame)
{
    if( frame->IPHeader->UDPHeader->dstport==bswap(DAYTIME)){
        UDPCoInfo cinfo;
        cinfo.remoteip=frame->IPHeader->srcip;
        cinfo.localport=DAYTIME;
        cinfo.remoteport=bswap(frame->IPHeader->UDPHeader->srcport);
        cinfo.protocol=TYPEUDP;
        char* data="I can't see a clock.";
        Send((byte*)data,20,&cinfo);
        return true;
    }
    return false;
}

void IPStack::CreateIPHeader(Ether* frame,word length,byte protocol)
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

