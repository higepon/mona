//$Id$
#pragma once
#include <monapi.h>
#include <sys/types.h>
#include <monapi/io.h>

namespace mones{

enum IP_PROTOCOL{
    TYPEICMP = 0x01,
    TYPEIGMP = 0x02,
    TYPETCP  = 0x06,
    TYPEUDP  = 0x11,
};

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
    uint8_t  type;
    uint8_t  code;
    uint16_t  chksum;
    uint16_t  idnum;
    uint16_t  seqnum;
    uint8_t  data[0];
};

struct UDP {
    uint16_t  srcport;
    uint16_t  dstport;
    uint16_t  len;
    uint16_t  chksum;
    uint8_t  data[0];
};

struct TCP {
    uint16_t  srcport;
    uint16_t  dstport;
    uint32_t seqnumber;
    uint32_t acknumber;
    uint8_t  offset;
    uint8_t  flags;
    uint16_t  window;
    uint16_t  chksum;
    uint16_t  urgent;
    uint8_t  data[0];
};

struct IP {
    uint8_t  verhead;
    uint8_t  tos;
    uint16_t  len;
    uint16_t  id;
    uint16_t  frag;
    uint8_t  ttl;
    uint8_t  prot;
    uint16_t  chksum;
    uint32_t srcip;
    uint32_t dstip;
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
    uint16_t  hardType;
    uint16_t  protType;
    uint8_t  hardAddrLen;
    uint8_t  protAddrLen;
    uint16_t  opeCode;
    uint8_t  srcMac[6];
    uint32_t srcIp;
    uint8_t  dstMac[6];
    uint32_t dstIp;
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
    uint8_t  dstmac[6];
    uint8_t  srcmac[6];
    uint16_t  type;
public:
    union{
        uint8_t data[DEFAULT_MTU_SIZE];
        IP   IPHeader[0];
        ARP  ARPHeader[0];
    };
};
#pragma pack(pop)

};
