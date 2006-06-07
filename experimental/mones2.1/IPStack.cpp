//$Id$
#include <pci/Pci.h>
#include "Net.h"
#include "NE2000.h"
#include "MonAMDpcn.h"
#include "LoopBack.h"
#include "IPStack.h"

using namespace mones;
using namespace MonAPI;

word ConnectionInfo::checksum(byte *data,word size)
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

void ConnectionInfo::CreateIPHeader(Ether* frame,word length)
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
////////////////////////////////////////////////
void ICMPCoInfo::CreateHeader(Ether* frame,byte* data, word size)
{
    ICMP* icmp=frame->IPHeader->ICMPHeader;
    icmp->type=type;
    icmp->code=0x00;
    icmp->chksum=0x0000;
    icmp->idnum=bswap(idnum);
    icmp->seqnum=bswap(seqnum);    
    memcpy(icmp->data,data,size);
    icmp->chksum=bswap(checksum((byte*)icmp,size+sizeof(ICMP)));
    CreateIPHeader(frame,size+sizeof(ICMP)+sizeof(IP));
}

int ICMPCoInfo::Strip(Ether* frame, byte** data)
{
   *data=frame->IPHeader->ICMPHeader->data;
   return bswap(frame->IPHeader->len)-sizeof(IP)-sizeof(ICMP);
}

/////////////////////////////////////////////////
void UDPCoInfo::CreateHeader(Ether* frame,byte* data, word size)
{
    IP* ip=frame->IPHeader; //for psedo header
    ip->ttl =0x00;
    ip->prot=TYPEUDP;
    ip->chksum=bswap(size+sizeof(UDP));
    UDP* udp=frame->IPHeader->UDPHeader;
    udp->srcport=bswap(localport);
    udp->dstport=bswap(remoteport);
    udp->len=bswap(size+sizeof(UDP));
    udp->chksum=0x0000;
    memcpy(frame->IPHeader->UDPHeader->data,data,size);
    udp->chksum=bswap(checksum((byte*)&(ip->ttl),size+sizeof(UDP)+12));
    CreateIPHeader(frame,size+sizeof(UDP)+sizeof(IP));
}

int UDPCoInfo::Strip(Ether* frame, byte** data)
{   
    *data=frame->IPHeader->UDPHeader->data;
    return bswap(frame->IPHeader->len)-sizeof(IP)-sizeof(UDP);
}

///////////////////////////////////////////////////

void TCPCoInfo::CreateHeader(Ether* frame,byte* data, word size)
{
    //HandShakeACTIV();       
    CreateIPHeader(frame,size+sizeof(TCP)+sizeof(IP));
}

int TCPCoInfo::Strip(Ether* frame, byte** data)
{
    *data=frame->IPHeader->TCPHeader->data;
    return bswap(frame->IPHeader->len)-sizeof(IP)-sizeof(TCP);
}

//   CLOSED       -> LISTENING @readAPI    ==
//   ESTABLISHED  -> FIN_WAIT1 @closeAPI   send FIN
//   ~ESTABLISHED -> CLOSED    @timerAPI   ==
bool TCPCoInfo::HandShakeACTV(Ether* frame)
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

bool TCPCoInfo::HandShakePASV(Ether* frame)
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

/////////////////////////////////////////////////

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
            ICMPCoInfo info;
            info.protocol=frame->IPHeader->prot;
            info.remoteip=frame->IPHeader->srcip;
            info.type=ECHOREPLY;
            info.idnum=bswap(icmp->idnum);
            info.seqnum=bswap(icmp->seqnum);
            Send(icmp->data,bswap(frame->IPHeader->len)-sizeof(IP)-sizeof(ICMP),&info);      
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
        //if( HandShakePASV(frame)){
        //    Dispose(n);
        //    GetDestination(n,cinfo);
        //}
        break;
    default:
        printf("unknown Packet type");
        Dispose(n);
        GetDestination(n,cinfo);
    }
    return true;
}

int IPStack::Send(byte* data,int size, ConnectionInfo* cinfo)
{              
    Ether* frame = nic->NewFrame(cinfo->remoteip);
    //ether header has been already filled.
    if( frame != NULL){   
        cinfo->CreateHeader(frame,data,size);
        nic->SendFrm(frame);
    }
    return 0;
}

void IPStack::read_bottom_half(int n,ConnectionInfo* cinfo)
{
    byte* data;
    monapi_cmemoryinfo* mi = monapi_cmemoryinfo_new();  
    if (mi != NULL){   
        Ether* frame=nic->RecvFrm(n);
        int size=cinfo->Strip(frame, &data);
        monapi_cmemoryinfo_create(mi,size, true);        
        if( mi != NULL ){
            memcpy(mi->Data,data,mi->Size);
            Message::reply(&(cinfo->msg), mi->Handle, mi->Size); 
        }
        Dispose(n);
        monapi_cmemoryinfo_delete(mi);
        memset(&(cinfo->msg),'\0',sizeof(MessageInfo));
    }else{
        Message::reply(&(cinfo->msg));
    }
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

void IPStack::PeriodicUpdate()
{

}

