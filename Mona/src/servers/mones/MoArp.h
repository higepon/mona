/*!
    \file  MoArp.h
    \brief ARPプロトコル実装クラス ヘッダ

    Copyright (c) 2004 Yamami
    All rights reserved.
    License=MIT/X Licnese

    \author  Yamami 
    \version $Revision$
    \date   create:2004/08/28 update:$Date$
*/
#ifndef _MONA_MOARP_
#define _MONA_MOARP_

#include <sys/types.h>
#include <monapi.h>

#include "MoPacUtl.h"
#include "MonesDefine.h"
#include "AbstractMonic.h"


/*! 
 *  \struct ARP_HEADER
 *  \brief ARPヘッダ構造体
 */
typedef struct{
    word hardType  __attribute__((packed));    //ﾊｰﾄﾞｳｪｱﾀｲﾌﾟ
    word protType  __attribute__((packed));    //ﾌﾟﾛﾄｺﾙﾀｲﾌﾟ
    byte  hardAddrLen  __attribute__((packed)); //ﾊｰﾄﾞｳｪｱｱﾄﾞﾚｽ長
    byte  protAddrLen  __attribute__((packed)); //ﾌﾟﾛﾄｺﾙｱﾄﾞﾚｽ長
    word opeCode  __attribute__((packed));     //ｵﾍﾟﾚｰｼｮﾝ
    byte  srcMac[6]  __attribute__((packed));   //送信元ﾊｰﾄﾞｳｪｱｱﾄﾞﾚｽ
    dword   srcIp  __attribute__((packed));       //送信元ﾌﾟﾛﾄｺﾙｱﾄﾞﾚｽ
    byte  dstMac[6]  __attribute__((packed));   //ﾀｰｹﾞｯﾄﾊｰﾄﾞｳｪｱｱﾄﾞﾚｽ
    dword   dstIp  __attribute__((packed));       //ﾀｰｹﾞｯﾄﾌﾟﾛﾄｺﾙｱﾄﾞﾚｽ
}ARP_HEADER;


/*! 
 *  \struct ARP_CACHE
 *  \brief ARPキャッシュ構造体
 */
typedef struct{
    dword ip;
    byte mac[6];
    char next;
}ARP_CACHE;


enum{
    /* Hard type. */
    HARD_TYPE_ETHER= 1,     /* Ethernet. */

    /* Protocol type. */
    PROT_TYPE_IP=0x0800,    /* IP. */

    /* Operation code. */
    OPE_CODE_ARP_REQ= 1,    // ARP要求
    OPE_CODE_ARP_REP= 2,    // ARP応答
    OPE_CODE_RARP_REQ=3,    // RARP要求
    OPE_CODE_RARP_REP=4,    // RARP応答
};


enum{
    ARP_CACHE_NUM=64,               /* ARPキャッシュ数。 */
    ARP_CACHE_HASH=ARP_CACHE_NUM/2, /* ARPキャッシュハッシュ数。 */
};


/*!
    MoArp class
*/
class MoArp
{
    
  public:
    
    //コンストラクタ
    MoArp();
    ~MoArp();
    void initArp(AbstractMonic*);

    //int getMac(int,uint,char*);
    int receiveArp(ARP_HEADER*);
    

  private:
    //ARP応答処理 処理
    void transArp(dword , byte*, word );
    
    
    //ARPキャッシュ未実装  !! キャッシュはMonaのHashを使う予定

    //NICドライバ
    AbstractMonic* insAbstractNic;

};

#endif
