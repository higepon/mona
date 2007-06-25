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
    uint8_t destmac[6];
    uint8_t header[4096];
    uint32_t size;
    uint16_t protocol;
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
    uint16_t srcport;
    uint16_t dstport;
    short len;
    uint16_t chksum;
    char   data[0];
} Header;
};

class IP {
public:
    enum {
        ICMP = 0x01,
        UDP = 0x11,
    };
    typedef struct{
        uint8_t  verhead;  /* バージョン、ヘッダ長。 */
        uint8_t  tos;      /* TOS. */
        uint16_t len;       /* トータル長。 */
        uint16_t id;        /* 識別番号。 */
        uint16_t frag;      /* フラグ、フラグメントオフセット。 */
        uint8_t  ttl;      /* Time to Live. */
        uint8_t  prot;     /* プロトコル番号。 */
        uint16_t chksum;    /* ヘッダチェックサム。 */
        uint32_t srcip;        /* 送り元IP。 */
        uint32_t dstip;        /* 宛先IP。 */
        char     data[0];
    } Header;
};

class Ether
{
public:
#pragma pack(2)
    typedef struct{
        uint8_t  dstmac[6];   // 送信先 MAC ID
        uint8_t  srcmac[6];   // 送信元 MAC ID
        uint16_t    type;     // フレームタイプ Frame type(DIX) or frame length(IEEE)
        uint8_t   data[0x600];// Data
    } Frame;
#pragma pack(0)
    enum
    {
        ARP = 0x806,
        IP  = 0x800,
    };
};


class Arp
{
public:
    typedef struct
    {
        uint16_t hardType  __attribute__((packed));
        uint16_t protType  __attribute__((packed));
        uint8_t  hardAddrLen  __attribute__((packed));
        uint8_t  protAddrLen  __attribute__((packed));
        uint16_t opeCode  __attribute__((packed));
        uint8_t  srcMac[6]  __attribute__((packed));
        uint32_t   srcIp  __attribute__((packed));
        uint8_t  dstMac[6]  __attribute__((packed));
        uint32_t   dstIp  __attribute__((packed));
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
    uint32_t getThreadID() const;
    void messageLoop();
    bool isStarted() {return started;}
    void exit();

private:
    void interrupt(MessageInfo* msg);

protected:
    uint8_t macAddress[6];
    HList<Ether::Frame*> frameList;
    uint32_t observerThread;
    uint32_t myID;
    Nic* nic;
    bool started;
    bool loopExit;
};
}; // namespace mones
#endif
