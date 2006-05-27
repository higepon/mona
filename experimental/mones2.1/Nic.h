#pragma once
#include "IPStack.h"
#include "Net.h"

namespace mones{

enum{
    DEFAULT_MTU_SIZE =1500,
    ETHER_HEADER_SIZE=14,
    ETHER_MIN_PACKET =46,      //MIN_SIZE
    ETHER_MAX_PACKET =DEFAULT_MTU_SIZE+ETHER_HEADER_SIZE,//MAX_SIZE
};

#pragma pack(push,2)
struct ARP {
    word  hardType;
    word  protType;
    byte  hardAddrLen;
    byte  protAddrLen;
    word  opeCode;
    byte  srcMac[6];
    dword srcIp;
    byte  dstMac[6];
    dword dstIp;
    enum{
        HARD_TYPE_ETHER= 1,
        PROTCOL_TYPE_IP=0x0800,
        OPE_CODE_ARP_REQ= 1,
        OPE_CODE_ARP_REP= 2,
        OPE_CODE_RARP_REQ=3,
        OPE_CODE_RARP_REP=4,
    };
};

struct Ether{
private:
    //I hate friend assignment. but.....
    //Creating an Ether-class from an provided IP-class is not a low cost procedure.
    //For reducing memory copy, Nic's I/O uses not IPHeader-class but Ether-class.
    //On the other hand, Ether frame header should be invisible from IP protocol layer,
    //so I use friend assignment.
    friend class ARPmanager;
    friend class Nic;
    byte  dstmac[6];
    byte  srcmac[6];
    word  type;
public:
    union{
        byte data[DEFAULT_MTU_SIZE];
        IP   IPHeader[0];
        ARP  ARPHeader[0];
    };
};
#pragma pack(pop)

class ARPmanager
{
    struct ARPRec{
        dword ip;
        byte mac[6];
    };
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
    void   getDstMacbyIP(dword address ,byte* dest);
    int    MakeArpReply(Ether*);
    int    Register(Ether*);

    //int    SetHeader(Ether*);
    void   DumpTable();
protected:
    char  devname[8];
    byte  macaddress[6];
    dword ipaddress;
    dword netmask;
    dword defaultroute;
    byte   registerd;
    enum{ CACHESIZE=0xFF };//Cass C
    ARPRec cache[CACHESIZE];
    Ether* Query(dword);
    int Lookup(byte*,dword);    
    word CalcFrameSize(Ether*);
};

class Nic : public ARPmanager
{
public:
    Nic();
    virtual int init() =0;
    virtual int interrupt() =0; 
    virtual void Send(Ether*)=0;
    Ether* Recv(int);    
    Ether* MakePKT(dword);
    byte  getIRQ() const {return this->irq;}
    int   getIOBase() const {return this->iobase;}
    void  setIRQ(byte n) {this->irq = n;}
    void  setIOBase(int addr) {this->iobase = addr & 0xFFFFFFE0;}
    void  enableNetwork() {monapi_set_irq(this->getIRQ(), MONAPI_TRUE, MONAPI_TRUE);}
    void  disableNetwork() {monapi_set_irq(this->getIRQ(), MONAPI_FALSE, MONAPI_TRUE);}
    void  getStatus(NetStatus* stat);
    enum{
        RX_INT     =0x0004,
        TX_INT     =0x0002,
        ER_INT     =0x0001,
    };
protected:  
    int   irq;
    int   iobase;
    word  mtu;
    //allocate & copy is slow.....
    //TODO use static allocated memory and one-copy.  
    HList<Ether*> rxFrameList;
    HList<Ether*> txFrameList;
};

};
