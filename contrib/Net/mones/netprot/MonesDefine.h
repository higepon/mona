/*!
    \file  MonesDefine.h
    \brief 各種グローバル定数宣言

    Copyright (c) 2004 Yamami
    All rights reserved.
    License=MIT/X License

    \author  Yamami 
    \version $Revision$
    \date   create:2004/08/28 update:$Date$
*/

#include <monapi/cmemoryinfo.h>

#ifndef _MONA_MONESDEFINE_
#define _MONA_MONESDEFINE_

/* Ethernet MAC層ヘッダ構成 */
#define SIZEOF_ETHERADDR    6

//DIX仕様フレームのフレームタイプ
#define ETHER_PROTO_IP      0x0800      // IPプロトコルフレーム
#define ETHER_PROTO_ARP     0x0806      // ARPプロトコルフレーム

#define etherhdr_dest_addr  0
#define etherhdr_src_addr   (etherhdr_dest_addr+SIZEOF_ETHERADDR)
#define etherhdr_type       (etherhdr_src_addr+SIZEOF_ETHERADDR)
#define SIZEOF_ETHERHDR     (etherhdr_type+2)


//IPヘッダプロトコルNo
enum{
    IPPROTO_IP,         /* Internet protocol. */
    IPPROTO_IPV6,       /* Internet Protocol Version 6. */
    IPPROTO_ICMP=1,     /* Control message protocol. */
    IPPROTO_IGMP=2,
    IPPROTO_RAW,        /* Raw IP Packets Protocol. */
    IPPROTO_TCP=6,      /* Transmission control protocol. */
    IPPROTO_UDP=17,     /* User datagram protocol. */

    INADDR_ANY=      0,             /* IPv4 local host address. */
    INADDR_BROADCAST=0xffffffff,    /* IPv4 broadcast address. */
};


/* IPヘッダ構成 */
#define     SIZEOF_IPADDR       4

#define     iphdr_version   0               /* 1 */
#define     iphdr_service   (iphdr_version+1)       /* 1 */
#define     iphdr_len   (iphdr_service+1)       /* 2 */
#define     iphdr_ident (iphdr_len+2)           /* 2 */
#define     iphdr_frags (iphdr_ident+2)         /* 2 */
#define     iphdr_ttl   (iphdr_frags+2)         /* 1 */
#define     iphdr_protocol  (iphdr_ttl+1)           /* 1 */
#define     iphdr_chksum    (iphdr_protocol+1)      /* 2 */
#define     iphdr_src_addr  (iphdr_chksum+2)        /* 4 */
#define     iphdr_dest_addr (iphdr_src_addr+4)      /* 4 */
#define     SIZEOF_IPHDR    (iphdr_dest_addr+4)



/*! 
 *  \struct IP_HEADER
 *  \brief IPヘッダ構造体
 */
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
}IP_HEADER;


enum{
    IP_HEAD_VERSION=4<<4,

    IP_HEAD_FRAG_ON= 0x2000,    /* フラグメントデータありフラグ。 */
    IP_HEAD_FRAG_NOT=0x4000,    /* フラグメント不可フラグ。 */
};


/*! 
 *  \struct DUMMY_HEADER
 *  \brief 疑似ヘッダ。TCP/UDPのチェックサム計算に利用される。
 */
typedef struct{
    uint32_t srcip;
    uint32_t dstip;
    uint8_t  tmp;
    uint8_t  prot;
    uint16_t len;
}DUMMY_HEADER;



//ICMP タイプ
//0 エコー応答（echo reply） 
//3 あて先不達（destination unreachable） 
//4 ソース・クエンチ（source quench、送信元抑制） 
//5 リダイレクト要求（redirect、経路変更要求） 
//8 エコー要求（echo request） 
//11 時間超過（time exceeded） 
//12 パラメータ異常（parameter problem） 
//13 タイムスタンプ要求（timestamp request） 
//14 タイムスタンプ応答（timestamp reply） 
//15 情報要求（information request） 
//16 情報応答（information reply） 
//17 アドレス・マスク要求（address mask request） 
//18 アドレス・マスク応答（address mask reply） 
enum{
    ICMP_TYPE_ECHOREP=0,
    ICMP_TYPE_ECHOREQ=8,
};


/*! 
 *  \struct ICMP_HEADER
 *  \brief ICMPヘッダ構造体
 */
typedef struct{
    unsigned char  type;        /* ICMPタイプ。 */
    unsigned char  code;        /* ICMPコード。 */
    unsigned short chksum;      /* チェックサム。 */
    char           data[32];     /* データ。 */
}ICMP_HEADER;



/*! 
 *  \struct UDP_HEADER
 *  \brief UDPヘッダ構造体
 */
 typedef struct{
    uint16_t srcport;
    uint16_t dstport;
    short len;
    uint16_t chksum;
    char   data[0];
}UDP_HEADER;


/*! 
 *  \struct TCP_HEADER
 *  \brief TCPヘッダ構造体
 */
typedef struct{
    uint16_t srcport;
    uint16_t dstport;
    uint32_t seqnum;
    uint32_t acknum;
    uint8_t reserved :4;
    uint8_t  headlen :4 ;
    uint8_t  flag;
    uint16_t wndsize;
    uint16_t chksum;
    uint16_t urgpoint;
    char    option[0];
}TCP_HEADER;


/*! 
 *  \struct TRANS_BUF_INFO
 *  \brief IP送信バッファ構造体
 */
struct TRANS_BUF_INFO{
    char  *data[3];     /* 送信フレームアドレス。 */
    int    size[3];     /* データフレームサイズ。 */
    uint16_t type;        /* フレームタイプ。 */
    uint32_t  ipType;      /* IPプロトコルタイプ。 */
};


/*! 
 *  \struct MAC_REPLY_WAIT
 *  \brief ARP要求待ち管理 構造体
 */
struct MAC_REPLY_WAIT{
    uint32_t ip;         //要求中IPアドレス
    int repFlg;       //Reply flag 0:待ち 1:完了
    int wait;         //ウェイト数 
    char mac[6];      //MACアドレス格納
    uint32_t  ipType;    /* IPプロトコルタイプ。 */
    //TRANS_BUF_INFO* ipPacketBuf; //IPパケットバッファ
    monapi_cmemoryinfo* ipPacketBuf01;  //IPパケットバッファ01
    monapi_cmemoryinfo* ipPacketBuf02;  //IPパケットバッファ02
};

/*! 
 *  \struct MONES_IP_REGIST
 *  \brief Mones登録情報
 */
struct MONES_IP_REGIST{
    uint32_t ip;         //通信中IP
    uint16_t port;       //通信中PORT
    uint32_t tid;        //スレッドID
};


#endif
