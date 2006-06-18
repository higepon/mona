//$Id$
#include <pci/Pci.h>
#include "Net.h"
#include "NE2000.h"
#include "MonAMDpcn.h"
#include "LoopBack.h"
#include "Dispatch.h"

using namespace mones;
using namespace MonAPI;

Dispatch::Dispatch()
{
    PciInf pciinfo;
    Pci* pcilib = new Pci();
    printf("\nIP address is defined in Dispatch constructor. Rewrite it as your environment.\n");
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
    for(int i=0;i<cinfolist.size();i++){
        delete cinfolist.get(i);
    }
    if( (nic != NULL) && (nic->getIRQ() != 0 ) ){
        syscall_remove_irq_receiver(this->nic->getIRQ());
        delete nic;
    }
}

bool Dispatch::initialize()
{    
    //For WellNkownServices.
    cinfolist.add(new ICMPCoInfo(this));
    cinfolist.add(new UDPCoInfo(this));
    cinfolist.add(new TCPCoInfo(this));
    if( nic != NULL ){
        if( nic->getIRQ()!=0 ) // with mask Interrrupt by higepon
            syscall_set_irq_receiver(this->nic->getIRQ(), SYS_MASK_INTERRUPT); 
         this->nic->enableNetwork();
        return true;
    }else return false;
}

void Dispatch::interrupt()
{ 
    int val= nic->interrupt();
    if( val & Nic::RX_INT ){
      //  printf("=RX\n");
        DoDispatch();
    }
    if(val & Nic::TX_INT){
      //  printf("=TX\n");
    }
    if( val & Nic::ER_INT){
        printf("=ERROR.\n");    
    }
}

void Dispatch::DoDispatch()
{
    int pktnumber=0;
    Ether* frame;
    byte* data;
    while( frame = nic->RecvFrm(pktnumber) ){
        for(int i=0;i<cinfolist.size(); i++){
            ConnectionInfo* cinfo =cinfolist.get(i);
            if( (cinfo!=NULL) && cinfo->IsMyPacket(frame)){
                if(  cinfo->msg.header == MSG_NET_READ ){
                    read_bottom_half(pktnumber,cinfo);
                    pktnumber--;
                }else if(i<3){ //pkt for well known services.
                    Dispose(pktnumber);
                    pktnumber--;
                }else if( cinfo->msg.header == MSG_NET_WRITE ){
                    write_bottom_half(pktnumber,cinfo);
                    byte* data;

                }
                if(cinfo->getType()==TYPETCP && cinfo->Strip(frame,&data) == 0 ){
                    Dispose(pktnumber);
                    pktnumber--;
                }
                //else pkt for opend but not reading.
            }else if(i>=3){
                Dispose(pktnumber); //pkt for unopend.
                pktnumber--;
            }
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

void Dispatch::write_bottom_half(int n,ConnectionInfo* cinfo)
{
    Ether* frame=nic->RecvFrm(n);
    if( frame != NULL ){
        if( frame->IPHeader->TCPHeader->flags == TCPCoInfo::ACK ){
            Message::reply(&(cinfo->msg));
            memset(&(cinfo->msg),'\0',sizeof(MessageInfo));
            printf("WRITE BOTTOM HALF\n");
        }
    }    
}    

void Dispatch::read_bottom_half(int n,ConnectionInfo* cinfo)
{
    byte* data;
    monapi_cmemoryinfo* mi = monapi_cmemoryinfo_new();  
    if (mi != NULL){   
        Ether* frame=nic->RecvFrm(n);
        if( frame != NULL ){
            IP* ip=frame->IPHeader;
            printf("[ID%d]\n",bswap(ip->id));
            int size=cinfo->Strip(frame, &data);
            if( size >0 ){
                monapi_cmemoryinfo_create(mi,size, true);
                if( mi != NULL ){
                    memcpy(mi->Data,data,mi->Size);    
                    printf("READ BOTTOM HALF\n");
                    Message::reply(&(cinfo->msg), mi->Handle, mi->Size);
                    memset(&(cinfo->msg),'\0',sizeof(MessageInfo));
                    if(cinfo->getType()==TYPETCP){
                        ((TCPCoInfo*)cinfo)->SendACK(frame);
                    }
                }
            }
            Dispose(n);
        }
        monapi_cmemoryinfo_delete(mi);
    }else{
        Message::reply(&(cinfo->msg));
    }
}

void Dispatch::RemoveConnection(ConnectionInfo* cinfo,dword delay)
{
    for(int i=0;i<cinfolist.size();i++){
        ConnectionInfo* c = cinfolist.get(i);
        if(cinfo->netdsc == c->netdsc){
            cinfo->disposedtick=syscall_get_tick()+delay;
            cinfo->disposed=true;
        }
    }
}

void Dispatch::PeriodicUpdate()
{
    //remove disposed connection.
    dword now=syscall_get_tick();
    for(int i=0;i<cinfolist.size();i++){
        ConnectionInfo* c=cinfolist.get(i);
        if( c->disposed==true && c->disposedtick  < now ){
            printf("remove\n");
            delete cinfolist.removeAt(i);
            i--;
        }
    }
}
