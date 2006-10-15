//$Id$
#include <pci/Pci.h>
#include "NE2000.h"
#include "MonADEC.h"
#include "MonAMDpcn.h"
#include "LoopBack.h"
#include "Dispatch.h"
#include "ICMPInfo.h"
#include "UDPInfo.h"
#include "TCPInfo.h"

using namespace mones;
using namespace MonAPI;

void Dispatch::readStatus(NetStatus* stat)
{    
    nic->getStatus(stat);  
    for(int i=0;i<cinfolist.size();i++){
        cinfolist.get(i)->Dump();
    }
}

Dispatch::Dispatch()
{
    packetid=0;
    serialno=1;
    PciInf pciinfo;
    Pci* pcilib = new Pci();
    printf("\nIP address is defined in Dispatch constructor. Rewrite it as your environment.\n");
    pcilib->CheckPciExist(MonADEC::VENDORID,MonADEC::DEVICEID,&pciinfo);
    if( pciinfo.Exist== 0){
        dword val=pcilib->ReadConfig(0, pciinfo.DeviceNo, 0,PCI_COMMAND,2);
        pcilib->WriteConfig(0,pciinfo.DeviceNo,0,PCI_COMMAND,2,val|0x4);
        nic=new MonADEC();    
        nic->setIP(192,168,0,6);  //VirtualPC. 
        nic->setIRQ(pciinfo.IrqLine);
        nic->setIOBase(pciinfo.BaseAd);
        if( nic->init() == 0 ){
            delete pcilib;
            return;
        }
        delete nic;
        nic=NULL;
    }

    pcilib->CheckPciExist(MonAMDpcn::VENDORID,MonAMDpcn::DEVICEID,&pciinfo);
    if( pciinfo.Exist== 0){
        dword val=pcilib->ReadConfig(0, pciinfo.DeviceNo, 0, PCI_COMMAND,2);
        pcilib->WriteConfig(0,pciinfo.DeviceNo,0, PCI_COMMAND,2,val|0x4);
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

    while( frame = nic->RecvFrm(pktnumber) ){
        bool hasClient=false;
        for(int i=0;i<cinfolist.size(); i++){
            L4Base* cinfo =cinfolist.get(i);
            if( cinfo->IsMyPacket(frame) ){
                hasClient= true;
                if( cinfo->IsProcessed(frame) ){
                    Dispose(pktnumber);
                    pktnumber--;
                    break;
                }
            }
        }
        if( hasClient==false ) {
            ReplyUnReach(frame);
            Dispose(pktnumber);
            pktnumber--;
        }    
        pktnumber++;
    }
}

void Dispatch::ReplyUnReach(Ether* frame)
{
    // printf("CreateInfo\n");
    switch(frame->IPHeader->prot)
    {
    case TYPEICMP:
    {
        ICMPCoInfo Icmp(this);
        Icmp.Reply(frame);
        break;
    }
    case TYPEUDP:
    {
        UDPCoInfo Udp(this);
        //Udp.ReplyUnReach(frame);
        break;
    }
    case TYPETCP:
    {
        TCPCoInfo Tcp(this);
        Tcp.ReplyUnReach(frame);
        break;
    }
    default:
        break;
    }
}

int Dispatch::Send(byte* data,int size, L4Base* cinfo)
{
    Ether* frame = nic->NewFrame(cinfo->remoteip);
    //header has been already filled.
    if( frame != NULL){
        cinfo->CreateHeader(frame,data,size);
        nic->SendFrm(frame);
    }
    return 0;
}

void Dispatch::RemoveInfo(L4Base* cinfo,dword delay)
{
    for(int i=0;i<cinfolist.size();i++){
        L4Base* c = cinfolist.get(i);
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
        L4Base* c=cinfolist.get(i);
        if( c->TimeoutCheck( now )){
        //}
        //if( c->disposed==true && c->disposedtick  < now ){
            //printf("remove\n");
            delete cinfolist.removeAt(i);
            i--;
        }
    }
    DoDispatch();
}
