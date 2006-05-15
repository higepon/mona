#pragma once
#include <monapi.h>
#include <sys/types.h>
#include <monapi/io.h>

namespace mones{

inline word  bswap(word w){ return (w>>8)|(w<<8); }

class Ether
{
public:
#pragma pack(2)
    typedef struct{
        byte  dstmac[6];
        byte  srcmac[6];
        word  type;
        byte  data[0x600];
        byte payloadsize;  
    } Frame;
#pragma pack(0)
    enum
    {
        ARP = 0x806,
        IP  = 0x800,
    };
};

class Nic
{
public:
    Nic();
    virtual ~Nic();
    virtual int init() =0;
    virtual void Send(Ether::Frame*)=0;
    virtual int interrupt() =0;
    virtual void getMacAddress(byte* dest) =0;
    Ether::Frame* Recv(int);
    byte  getIRQ() const {return this->irq;}
    int   getIOBase() const {return this->iobase;}
    void  setIRQ(byte n) {this->irq = n;}
    void  setIOBase(int addr) {this->iobase = addr & 0xFFFFFFE0;}
    void  enableNetwork() {monapi_set_irq(this->getIRQ(), MONAPI_TRUE, MONAPI_TRUE);}
    void  disableNetwork() {monapi_set_irq(this->getIRQ(), MONAPI_FALSE, MONAPI_TRUE);}
    enum{
        RX_INT     =0x0004,
        TX_INT     =0x0002,
        ER_INT     =0x0001,
    };    
    void  setIP(byte a,byte b,byte c,byte d){ myIP=((d<<24)|(c<<16)|(b<<8)|a);}
    dword getIP(){ return myIP; };
protected:
    //allocate & copy is slow.....
    //TODO use static allocated memory and one-copy.  
    HList<Ether::Frame*> rxFrameList;
    HList<Ether::Frame*> txFrameList;
    dword myIP;
    int   irq;
    int   iobase;
};

};
