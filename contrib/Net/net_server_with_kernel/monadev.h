#ifndef __MONADEV_H__
#define __MONADEV_H__

#include "stdint.h"


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

void monadev_init(void);
unsigned int monadev_read(void);
void monadev_send(void);

uint32_t nic_read(Ether::Frame* frame);
uint32_t nic_write(OutPacket* packet);


#endif /* __MONADEV_H__ */
