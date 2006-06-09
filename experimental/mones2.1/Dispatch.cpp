//$Id$
#include <pci/Pci.h>
#include "Net.h"
#include "NE2000.h"
#include "MonAMDpcn.h"
#include "LoopBack.h"
#include "Dispatch.h"

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

void ConnectionInfo::CreateIPHeader(Ether* frame,word length,byte protocol)
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
    CreateIPHeader(frame,size+sizeof(ICMP)+sizeof(IP),TYPEICMP);
}

int ICMPCoInfo::Strip(Ether* frame, byte** data)
{
   *data=frame->IPHeader->ICMPHeader->data;
   return bswap(frame->IPHeader->len)-sizeof(IP)-sizeof(ICMP);
}

bool ICMPCoInfo::IsMyPacket(Ether* frame)
{
    if( WellKnownSVCreply(frame) ){
        return false;
    }else if ( TYPEICMP  == frame->IPHeader->prot ){
        ICMP* icmp=frame->IPHeader->ICMPHeader;
         if( ECHOREPLY == icmp->type  &&
            remoteip  == frame->IPHeader->srcip &&
            idnum     == bswap(icmp->idnum) &&
            seqnum    == bswap(icmp->seqnum) )
        {
            return true;
        }
    }
    return false;
}

bool ICMPCoInfo::WellKnownSVCreply(Ether* frame)
{
    if( TYPEICMP == frame->IPHeader->prot ){
        ICMP* icmp=frame->IPHeader->ICMPHeader;
        if( ECHOREQUEST==icmp->type ){
            ICMPCoInfo info(dispatcher);
            info.remoteip=frame->IPHeader->srcip;
            info.type=ECHOREPLY;
            info.idnum=bswap(icmp->idnum);
            info.seqnum=bswap(icmp->seqnum);
            dispatcher->Send(icmp->data,bswap(frame->IPHeader->len)-sizeof(IP)-sizeof(ICMP),&info);
            return true;
        }
    }
    return false;
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
    CreateIPHeader(frame,size+sizeof(UDP)+sizeof(IP),TYPEUDP);
}

int UDPCoInfo::Strip(Ether* frame, byte** data)
{   
    *data=frame->IPHeader->UDPHeader->data;
    return bswap(frame->IPHeader->len)-sizeof(IP)-sizeof(UDP);
}

bool UDPCoInfo::IsMyPacket(Ether* frame)
{
    if( WellKnownSVCreply(frame) ){
        return false;
    }else if( TYPEUDP    == frame->IPHeader->prot &&
              remoteip   == frame->IPHeader->srcip &&
              localport  == bswap(frame->IPHeader->UDPHeader->dstport) &&
              remoteport == bswap(frame->IPHeader->UDPHeader->srcport)
            ){
        return true;
    }
    return false;
}

bool UDPCoInfo::WellKnownSVCreply(Ether* frame)
{
    if( frame->IPHeader->UDPHeader->dstport==bswap(DAYTIME)){
        UDPCoInfo cinfo(dispatcher);
        cinfo.remoteip=frame->IPHeader->srcip;
        cinfo.localport=DAYTIME;
        cinfo.remoteport=bswap(frame->IPHeader->UDPHeader->srcport);
        char* data="I can't see a clock.";    
        dispatcher->Send((byte*)data,20,&cinfo);
        printf("T\n");
        return true;
    }
    return false;
}

///////////////////////////////////////////////////

void TCPCoInfo::CreateHeader(Ether* frame,byte* data, word size)
{
    //HandShakeACTIV();       
    CreateIPHeader(frame,size+sizeof(TCP)+sizeof(IP),TYPETCP);
}

int TCPCoInfo::Strip(Ether* frame, byte** data)
{
    *data=frame->IPHeader->TCPHeader->data;
    return bswap(frame->IPHeader->len)-sizeof(IP)-sizeof(TCP);
}

bool TCPCoInfo::IsMyPacket(Ether* frame)
{
    if( WellKnownSVCreply(frame) ){
        return false;
    }else if( TYPETCP    == frame->IPHeader->prot &&
              remoteip   == frame->IPHeader->srcip &&
              localport  == bswap(frame->IPHeader->TCPHeader->dstport) &&
              remoteport == bswap(frame->IPHeader->TCPHeader->srcport)
            ){
        return true;
    }
    return false;
}

bool TCPCoInfo::WellKnownSVCreply(Ether* frame)
{
    if( frame->IPHeader->UDPHeader->dstport==bswap(DAYTIME)){
        UDPCoInfo cinfo(dispatcher);
        cinfo.remoteip=frame->IPHeader->srcip;
        cinfo.localport=DAYTIME;
        cinfo.remoteport=bswap(frame->IPHeader->UDPHeader->srcport);
        char* data="I can't see a clock.";    
        dispatcher->Send((byte*)data,20,&cinfo);
        return true;
    }
    return false;
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

Dispatch::Dispatch()
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

Dispatch::~Dispatch()
{
    if( (nic != NULL) && (nic->getIRQ() != 0 ) ){
        syscall_remove_irq_receiver(this->nic->getIRQ());
        delete nic;
    }
}

bool Dispatch::initialize()
{    
    if( nic != NULL ){
        if( nic->getIRQ()!=0 ) // with mask Interrrupt by higepon
            syscall_set_irq_receiver(this->nic->getIRQ(), SYS_MASK_INTERRUPT); 
         this->nic->enableNetwork();
        return true;
    }else return false;
}

void Dispatch::DoDispatch()
{
    int pktnumber=0;
    Ether* frame;
    while( frame = nic->RecvFrm(pktnumber) ){
        for(int i=0;i<cinfolist.size(); i++){
            ConnectionInfo* cinfo =cinfolist.get(i);
            if( (cinfo!=NULL) && cinfo->IsMyPacket(frame) ){
                if( cinfo->msg.header == MSG_NET_READ ){
                    read_bottom_half(pktnumber,cinfo);
                    pktnumber--;
                }else{
                    printf("opend but not reading... waiting reatry.\n");
                }
            }else{
                Dispose(pktnumber); //pkt for unopend.
                pktnumber--;
            }
        }

        //WellKnownServices.
        ICMPCoInfo icmp(this);
        UDPCoInfo udp(this);
        TCPCoInfo tcp(this);
        if( icmp.WellKnownSVCreply(frame) ||
            udp.WellKnownSVCreply(frame) ||
            tcp.WellKnownSVCreply(frame)  
          ){
            Dispose(pktnumber);
            pktnumber--;
        }

        pktnumber++;
    }
}

int Dispatch::Send(byte* data,int size, ConnectionInfo* cinfo)
{              
    Ether* frame = nic->NewFrame(cinfo->remoteip);
    //ether header has been already filled.
    if( frame != NULL){   
        cinfo->CreateHeader(frame,data,size);
        nic->SendFrm(frame);
    }
    return 0;
}

void Dispatch::read_bottom_half(int n,ConnectionInfo* cinfo)
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

void Dispatch::PeriodicUpdate()
{

}
