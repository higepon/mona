//$Id$
#pragma once
#include <monapi.h>
#include <sys/types.h>
#include <monapi/io.h>
#include "Net.h"
#include "Packet.h"
namespace mones{

inline word  bswap(word w){ return (w>>8)|(w<<8); }

class ARPmanager
{
public:
    enum{
        TYPEARP=0x806,
        TYPEIP =0x800,
    };
    ARPmanager();
    virtual ~ARPmanager();
    void   getMacAddress(byte* dest){memcpy(dest,macaddress,6);};
    void   setIP(byte a,byte b,byte c,byte d){ ipaddress=((d<<24)|(c<<16)|(b<<8)|a);}
    dword  getIP(){ return ipaddress; };    
    void   DumpTable();
protected:      
    int    MakeArpReply(Ether*);
    int    Register(Ether*);
    char  devname[64]; 
    word CalcFrameSize(Ether*);
    Ether* Query(dword);
    int Lookup(byte*,dword);  
    dword ipaddress;    
    byte  macaddress[6];
    dword netmask;
    dword defaultroute;
private:    
    struct ARPRec{
        dword ip;
        byte mac[6];
    };
    byte   registerd;
    enum{ CACHESIZE=0xFF };//Cass C
    ARPRec cache[CACHESIZE];

};

class Nic : public ARPmanager
{
public:
    Nic();
    virtual int init() =0;
    virtual int interrupt() =0; 
    virtual void SendFrm(Ether*)=0;
    Ether* RecvFrm(int);    
    Ether* NewFrame(dword);
    void  Delete(int);
    byte  getIRQ() const {return this->irq;}
    int   getIOBase() const {return this->iobase;}
    void  setIRQ(byte n) {this->irq = n;}
    void  setIOBase(int addr) {this->iobase = addr & 0xFFFFFFE0;}
    void  enableNetwork() {if(this->getIRQ()!=0)monapi_set_irq(this->getIRQ(), MONAPI_TRUE, MONAPI_TRUE);}
    void  disableNetwork() {if(this->getIRQ()!=0)monapi_set_irq(this->getIRQ(), MONAPI_FALSE, MONAPI_TRUE);}
    void  getStatus(NetStatus* stat);
    enum{
        RX_INT     =0x0004,
        TX_INT     =0x0002,
        ER_INT     =0x0001,
    };    
protected:
    //allocate & copy is slow.....
    //TODO use static allocated memory and one-copy.  
    HList<Ether*> rxFrameList;
    HList<Ether*> txFrameList;
    int   irq;
    int   iobase;
    word  mtu;

};

};
