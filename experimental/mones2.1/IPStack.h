//$Id$
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

inline word  bswap(word w){ return (w>>8)|(w<<8); }

#pragma pack(push,2)

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
};

class IPStack
{
public:
    bool Match(byte* ,IP*);
    word checksum(byte*,word);
    void FillIPHeader(IP*);
    void dumpPacket(IP*); 
    void FillICMPHeader(ICMP*,ICMP*);
};

};
#pragma pack(pop)
