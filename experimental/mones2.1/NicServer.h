#include <monapi.h>
#include <sys/types.h>
#include "Nic.h"
#include "NicFactory.h"

#ifndef _MONA_MONES_NIC_SERVER_
#define _MON

namespace mones {

#define MSG_FRAME_READY 0x12345678
#define MSG_FRAME_WRITE 0x87654321
#define MSG_FRAME_READ 0x87654322
#define MSG_GET_MAC_ADDRESS 0x41530000

typedef struct {
    byte destmac[6];
    byte header[4096];
    dword size;
    word protocol;
} OutPacket;

class Icmp {
public :
#pragma pack(2)
    typedef struct{
        unsigned char  type;        /* ICMPタイプ。 */
        unsigned char  code;        /* ICMPコード。 */
        unsigned short chksum;      /* チェックサム。 */
        char           data[32];     /* データ。 */
    } Header;
#pragma pack(0)
};

class UDP {
public:
 typedef struct{
    word  srcport;
    word  dstport;
    word len;
    word  chksum;
    char  data[0];
} Header;
};

class IP {
public:
    enum {
        ICMP = 0x01,
        IGMP = 0x02,
        TCP  = 0x06,
        UDP  = 0x11,
    };
    typedef struct{
        byte  verhead;  /* バージョン、ヘッダ長。 */
        byte  tos;      /* TOS. */
        word  len;      /* トータル長。 */
        word  id;       /* 識別番号。 */
        word  frag;     /* フラグ、フラグメントオフセット。 */
        byte  ttl;      /* Time to Live. */
        byte  prot;     /* プロトコル番号。 */
        word  chksum;   /* ヘッダチェックサム。 */
        dword srcip;    /* 送り元IP。 */
        dword dstip;    /* 宛先IP。 */
        char  data[0];
    } Header;
};



class Arp
{
public:
    typedef struct
    {
        word  hardType  __attribute__((packed));
        word  protType  __attribute__((packed));
        byte  hardAddrLen  __attribute__((packed));
        byte  protAddrLen  __attribute__((packed));
        word  opeCode  __attribute__((packed));
        byte  srcMac[6]  __attribute__((packed));
        dword srcIp  __attribute__((packed));
        byte  dstMac[6]  __attribute__((packed));
        dword dstIp  __attribute__((packed));
    } Header;

    enum
    {
        HARD_TYPE_ETHER= 1,
        PROTCOL_TYPE_IP=0x0800,
        /* Operation code. */
        OPE_CODE_ARP_REQ= 1,
        OPE_CODE_ARP_REP= 2,
        OPE_CODE_RARP_REQ=3,
        OPE_CODE_RARP_REP=4,
    };
};

void SetFrameToSharedMemory(Ether::Frame* frame);
void GetFrameFromSharedMemory(Ether::Frame* frame);

class NicServer
{
public:
    NicServer();
    virtual ~NicServer();

public:
    bool initialize();
    dword getThreadID() const;
    void messageLoop();
    bool isStarted() {return started;}
    void exit();

private:
    void interrupt(MessageInfo* msg);
    int  ARPhandler(Ether::Frame*);
    void dumpPacket(Ether::Frame*);
protected:
    byte macAddress[6];
    dword observerThread;
    dword myID;
    Nic* nic;
    bool started;
    bool loopExit;
};
}; // namespace mones
#endif
