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


//DIX仕様フレームのフレームタイプ
#define     DIX_TYPE_IP     0x0800    // IPプロトコルフレーム
#define     DIX_TYPE_ARP    0x0806    // ARPプロトコルフレーム

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
