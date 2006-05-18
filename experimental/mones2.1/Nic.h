#pragma once
#include <monapi.h>
#include <sys/types.h>
#include <monapi/io.h>

namespace mones{

inline word  bswap(word w){ return (w>>8)|(w<<8); }

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

struct ICMP {
    byte  type;
    byte  code;
    word  chksum;
    byte  data[0];
};

struct UDP {
    word  srcport;
    word  dstport;
    word  len;
    word  chksum;
    byte  data[0];
};

struct TCP {
    word  srcport;
    word  dstport;
    dword seqnumber;
    dword acknumber;
    byte  reserved;
    byte  flag;
    word  window;
    word  chksum;
    word  purgent;
    byte  data[0];
};

struct IP {
    byte  verhead;
    byte  tos;
    word  len;
    word  id;
    word  frag;
    byte  ttl;
    byte  prot;
    word  chksum;
    dword srcip;
    dword dstip;
    union{
        ICMP ICMPHeader[0];
        TCP  TCPHeader[0];
        UDP  UDPHeader[0];
    };
    enum {
        TYPEICMP = 0x01,
        TYPEIGMP = 0x02,
        TYPETCP  = 0x06,
        TYPEUDP  = 0x11,
    };
};

struct Ether{
private:
    //I hate friend assignment. but.....
    //Creating an Ether-class from an provided IP-class is not a low cost procedure.
    //For reducing memory copy, Nic's I/O uses not IPHeader-class but Ether-class.
    //On the other hand, Ether frame header should be invisible from IP protocol layer,
    //so I use fried assignment.
    friend class ARPhandler;
    friend class Nic;
    byte  dstmac[6];
    byte  srcmac[6];
    word  type;
public:
    union{
        byte data[1500];
        IP   IPHeader[0];
        ARP  ARPHeader[0];
    };
    byte payloadsize;
};

#pragma pack(pop)



class ARPhandler
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
    ARPhandler();
    virtual ~ARPhandler();
    void  getMacAddress(byte* dest){memcpy(dest,macaddress,6);};
    void  setIP(byte a,byte b,byte c,byte d){ ipaddress=((d<<24)|(c<<16)|(b<<8)|a);}
    dword getIP(){ return ipaddress; };    
    void  getDstMacbyIP(dword address ,byte* dest);
    void  ARPreply(Ether*);
    int   SetHeader(Ether*);
    void  DumpTable();
    virtual void Send(Ether*)=0;
protected:
    byte  macaddress[6];
    dword ipaddress;
    dword netmask;
    dword defaultroute;

    int head;
    enum{ CACHESIZE=0xFF };
    ARPRec cache[CACHESIZE];
    void Query(dword);
    int Lookup(byte*,dword);
};

class Nic : public ARPhandler
{
public:
    Nic();
    virtual int init() =0;
    virtual int interrupt() =0;
    Ether* Recv(int);
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
protected:  
    int   irq;
    int   iobase;
    //allocate & copy is slow.....
    //TODO use static allocated memory and one-copy.  
    HList<Ether*> rxFrameList;
    HList<Ether*> txFrameList;
};

};
