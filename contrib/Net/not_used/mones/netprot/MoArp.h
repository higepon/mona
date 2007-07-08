/*!
    \file  MoArp.h
    \brief ARPプロトコル実装クラス ヘッダ

    Copyright (c) 2004 Yamami
    All rights reserved.
    License=MIT/X License

    \author  Yamami 
    \version $Revision$
    \date   create:2004/08/28 update:$Date$
*/
#ifndef _MONA_MOARP_
#define _MONA_MOARP_

#include <sys/types.h>
#include <sys/HashMap.h>
#include <monapi.h>

#include "MoPacUtl.h"
#include "MonesDefine.h"
#include <nicdrv/AbstractMonic.h>

using namespace MonAPI;

/*! 
 *  \struct ARP_HEADER
 *  \brief ARPヘッダ構造体
 */
typedef struct{
    uint16_t hardType  __attribute__((packed));    //ﾊｰﾄﾞｳｪｱﾀｲﾌﾟ
    uint16_t protType  __attribute__((packed));    //ﾌﾟﾛﾄｺﾙﾀｲﾌﾟ
    uint8_t  hardAddrLen  __attribute__((packed)); //ﾊｰﾄﾞｳｪｱｱﾄﾞﾚｽ長
    uint8_t  protAddrLen  __attribute__((packed)); //ﾌﾟﾛﾄｺﾙｱﾄﾞﾚｽ長
    uint16_t opeCode  __attribute__((packed));     //ｵﾍﾟﾚｰｼｮﾝ
    uint8_t  srcMac[6]  __attribute__((packed));   //送信元ﾊｰﾄﾞｳｪｱｱﾄﾞﾚｽ
    uint32_t   srcIp  __attribute__((packed));       //送信元ﾌﾟﾛﾄｺﾙｱﾄﾞﾚｽ
    uint8_t  dstMac[6]  __attribute__((packed));   //ﾀｰｹﾞｯﾄﾊｰﾄﾞｳｪｱｱﾄﾞﾚｽ
    uint32_t   dstIp  __attribute__((packed));       //ﾀｰｹﾞｯﾄﾌﾟﾛﾄｺﾙｱﾄﾞﾚｽ
}ARP_HEADER;



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

    int receiveArp(ARP_HEADER*);
    uint8_t* searchCache(uint32_t );
    int getMac(uint32_t ,uint8_t*);

    //TO DO Yamami とりあえずPublicメンバ ARPクラスが保持すべきか？
    //ARP要求待ちリスト
    List<MAC_REPLY_WAIT*>* macWaitList;


  private:
    //ARP応答処理 処理
    void transArp(uint32_t , uint8_t*, uint16_t );
    void addArpCache(uint32_t , uint8_t*);
    //ARP応答受信処理
    int receiveReply(uint32_t ,uint8_t*);
    
    //ARPキャッシュ用 HashMap
    HashMap<uint8_t*>* ArpCache;


    //NICドライバ
    AbstractMonic* insAbstractNic;

};

#endif
