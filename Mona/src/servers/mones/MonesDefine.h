/*!
    \file  MonesDefine.h
    \brief 各種グローバル定数宣言

    Copyright (c) 2004 Yamami
    All rights reserved.
    License=MIT/X Licnese

    \author  Yamami 
    \version $Revision$
    \date   create:2004/08/28 update:$Date$
*/

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


#endif
