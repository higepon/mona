//$Id$
#pragma once
#include <monapi.h>
#include <sys/types.h>
#include <monapi/io.h>
#include "Net.h"
#include "Packet.h"
namespace mones{

inline uint16_t  bswap(uint16_t w){ return (w>>8)|(w<<8); }
inline uint32_t bswapl(uint32_t dw){ return (dw>>24)|((dw&0x00FF0000)>>8)|((dw&0x0000FF00)<<8)|(dw<<24);}

class ARPmanager
{
public:
    enum{
        TYPEARP=0x806,
        TYPEIP =0x800,
    };
    ARPmanager();
    virtual ~ARPmanager();
    void   getMacAddress(uint8_t* dest){memcpy(dest,macaddress,6);};
    void   setIP(uint8_t a,uint8_t b,uint8_t c,uint8_t d){ ipaddress=((d<<24)|(c<<16)|(b<<8)|a);}
    uint32_t  getIP(){ return ipaddress; };    
    void   DumpTable();
protected:      
    int    MakeArpReply(Ether*);
    int    Register(Ether*);
    char  devname[64]; 
    uint16_t CalcFrameSize(Ether*);
    Ether* Query(uint32_t);
    int Lookup(uint8_t*,uint32_t);  
    uint32_t ipaddress;    
    uint8_t  macaddress[6];
    uint32_t netmask;
    uint32_t defaultroute;
private:    
    struct ARPRec{
        uint32_t ip;
        uint8_t mac[6];
    };
    uint8_t   registerd;
    enum{ CACHESIZE=0xFF };//Cass C
    ARPRec cache[CACHESIZE];

};

class Nic : public ARPmanager
{
public:
    Nic();
    ~Nic();
    virtual int init() =0;
    virtual int interrupt() =0; 
    virtual void SendFrm(Ether*)=0;
    Ether* RecvFrm(int);    
    Ether* NewFrame(uint32_t);
    void  Delete(int);
    uint8_t  getIRQ() const {return this->irq;}
    int   getIOBase() const {return this->iobase;}
    void  setIRQ(uint8_t n) {this->irq = n;}
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
    uint16_t  mtu;
    uint8_t* dma_head;
    int   dma_size;
    uint8_t* AllocateDmaPages(int);
};

};
