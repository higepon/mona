#ifndef _MONA_MONES_NIC_
#define _MONA_MONES_NIC_

#include <monapi.h>
#include <sys/types.h>
#include <monapi/io.h>

namespace mones {

class Ether
{
public:
#pragma pack(2)
    typedef struct{
        byte  dstmac[6];   //dest mac
        byte  srcmac[6];   //src mac
        word  type;        //type or length
        byte  data[0x600]; // Data
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
    virtual Ether::Frame* Recv(int)=0;
    virtual int interrupt() =0;
    virtual void getMacAddress(byte* dest) =0;

	byte  getIRQ() const {return this->irq;}
    int   getIOBase() const {return this->iobase;}
    void  setIRQ(byte n) {this->irq = n;}
    void  setIOBase(int addr) {this->iobase = addr & 0xFFFFFFE0;}
    void enableNetwork() {monapi_set_irq(this->getIRQ(), MONAPI_TRUE, MONAPI_TRUE);}
    void disableNetwork() {monapi_set_irq(this->getIRQ(), MONAPI_FALSE, MONAPI_TRUE);}
    enum{
        RX_INT     =0x0004,
        TX_INT     =0x0002,
        ER_INT     =0x0001,
    };	
    void setIP(byte a,byte b,byte c,byte d){ myIP=((d<<24)|(c<<16)|(b<<8)|a);}
    dword getIP(){ return myIP; };
//protected:
    HList<Ether::Frame*> rxFrameList;
protected:
    HList<Ether::Frame*> txFrameList;
	dword myIP;
    int   irq;
    int   iobase;
};

};
#endif
