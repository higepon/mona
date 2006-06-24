//$Id$
#pragma once
#include <monapi.h>
#include <sys/types.h>
#include <monapi/io.h>

namespace mones{

enum ICMPTYPE {
    ECHOREPLY   =0x0,
    DESTUNREACH =0x3,
    SOURCEQUENCH=0x4,
    REDIRECT    =0x5,
    ALTHOSTADDR =0x6,
    ECHOREQUEST =0x8,
    ROUTERADV   =0x9,
    ROUTERSELECT=0xA,
    TIMEEXCEEDED=0xB
};

#pragma pack(push,2)
struct ICMP {
    byte  type;
    byte  code;
    word  chksum;
    word  idnum;
    word  seqnum;
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
    byte  offset;
    byte  flags;
    word  window;
    word  chksum;
    word  urgent;
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
};

enum{
    DEFAULT_MTU_SIZE =1500,
    ETHER_HEADER_SIZE=14,
    ETHER_MIN_PACKET =46,      //MIN_SIZE
    ETHER_MAX_PACKET =DEFAULT_MTU_SIZE+ETHER_HEADER_SIZE,//MAX_SIZE
};

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

};
