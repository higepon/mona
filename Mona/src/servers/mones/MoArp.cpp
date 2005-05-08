/*!
    \file   MoArp.cpp
    \brief  ARPプロトコル実装クラス

    Copyright (c) 2004 Yamami
    All rights reserved.
    License=MIT/X License

    \author  Yamami
    \version $Revision$
    \date   create:2004/08/28 update:$Date$
*/

/*! \class MoArp
 *  \brief ARPプロトコル実装クラス
 */



#include "MoArp.h"
#include "MonesConfig.h"
#include "MonesGlobal.h"

/*!
    \brief initialize
         MoArp コンストラクタ
    \author Yamami
    \date   create:2004/08/28 update:
*/
MoArp::MoArp()
{
    //ARPキャッシュ初期化
    ArpCache = new HashMap<byte*>(ARP_CACHE_NUM);
    //ARP要求待ちリスト初期化
    macWaitList = new HList<MAC_REPLY_WAIT*>();
}

/*!
    \brief initialize
         MoArp initArp
    \author Yamami
    \param  AbstractMonic *pminsNic [in] NICクラスへのポインタ
    \date   create:2004/08/28 update:
*/
void MoArp::initArp(AbstractMonic *pminsNic ) 
{
    //NICクラスを保持
    insAbstractNic = pminsNic;
    return;
}


/*!
    \brief initialize
         MoArp デスクトラクタ
    \author Yamami
    \date   create:2004/08/28 update:
*/
MoArp::~MoArp() 
{

}


/*!
    \brief receiveArp
         ARPプロトコル受信 処理
    \param  ARP_HEADER *arpHead [in] ARPヘッダへのポインタ
    \return int 結果 
        
    \author Yamami
    \date   create:2004/08/28 update:$Date$
*/
int MoArp::receiveArp(ARP_HEADER *arpHead)
{
    
    int rest=0;
    
    // IPの確認
    // 自己IP宛の要求で無ければ、何もしない。
    if(arpHead->dstIp!=MoPacUtl::swapLong(G_MonesCon.getGl_myIpAdr() )){
        return 0;
    }

    // オペレーションコードの確認
    switch(MoPacUtl::swapShort(arpHead->opeCode))
    {
        //ARP要求
        case OPE_CODE_ARP_REQ:
            transArp(arpHead->srcIp,arpHead->srcMac, OPE_CODE_ARP_REP);
            
            //要求を受けた分も、キャッシュに登録
            addArpCache(arpHead->srcIp,arpHead->srcMac); 
            
            break;
            
        //ARP応答
        case OPE_CODE_ARP_REP:
            rest=receiveReply(arpHead->srcIp,arpHead->srcMac);
            break;
        
        //RARP要求
        case OPE_CODE_RARP_REQ:
        //RARP応答
        case OPE_CODE_RARP_REP:
            break;
    }

    return rest;

}



/*!
    \brief receiveReply
         ARP受信処理
    \param  dword srcip [in] 送信元IPアドレス
    \param  byte *mac [in] 送信元MACアドレス
    \return void 無し 
        
    \author Yamami
    \date   create:2004/11/15 update:$Date$
*/
int MoArp::receiveReply(dword srcip,byte *mac)
{
    
    MAC_REPLY_WAIT* nowWait;
    int findFlag = 0;
    
    //ARP要求待ちリストの検索
    for (int i = 0; i < macWaitList->size() ; i++) {
        nowWait = macWaitList->get(i);
        
        nowWait->wait++;
        if(nowWait->ip == srcip){
            //要求待ちのIPなら
            //MACアドレスコピー
            memcpy(nowWait->mac ,mac ,6);
            nowWait->repFlg = 1;

            //ARPキャッシュ登録
            addArpCache(srcip,mac); 
            
            //findFlagオン
            findFlag = 1;
        }
    }
    
    if(findFlag == 1){
        //Mones自身へメッセージ通知
        MessageInfo info;
        dword targetID = Message::lookupMainThread("MONES.EX5");  //TO DO この探し方良くないよなー
        // create message
        Message::create(&info, MSG_MONES_WAKEUP_ARP_WAIT, 0, 0, 0, NULL);
        Message::send(targetID, &info);
    }
    
    return 0;
}


//static char broadcastMac[]={0xff,0xff,0xff,0xff,0xff,0xff};
static byte broadcastMac[]={0xff,0xff,0xff,0xff,0xff,0xff};

/*!
    \brief transArp
         ARP送信処理 (ARP応答、ARP要求)
    \param  dword dstip [in] 送信先IPアドレス
    \param  byte *dstmac [in] 送信先MACアドレス
    \param  word opecode [in] オペレーションコード
    \return void 無し 
        
    \author Yamami
    \date   create:2004/08/28 update:$Date$
*/
void MoArp::transArp(dword dstip, byte *dstmac, word opecode)
{
    ARP_HEADER head;

    //応答用 ARPヘッダフレーム作成
    head.hardType=MoPacUtl::swapShort(HARD_TYPE_ETHER);
    head.protType=MoPacUtl::swapShort(PROT_TYPE_IP);
    head.hardAddrLen=6;
    head.protAddrLen=4;
    head.opeCode=MoPacUtl::swapShort(opecode);
    memcpy(head.srcMac,insAbstractNic->ether_mac_addr,6);
    head.srcIp=MoPacUtl::swapLong(G_MonesCon.getGl_myIpAdr() );
    if(memcmp(dstmac,broadcastMac,6)==0){
        head.srcIp=MoPacUtl::swapLong(G_MonesCon.getGl_myIpAdr() );
    }else{
        memcpy(head.dstMac,dstmac,6);
    }
    head.dstIp=dstip;  //ここは受け取ったまま返却するので、エンディアン変換は不要

    //送信処理 ここでは、直接ドライバをコール
    insAbstractNic->frame_output((byte *)&head , dstmac , sizeof(head) , ETHER_PROTO_ARP);

}


/*!
    \brief addArpCache
         ARPキャッシュ登録処理
    \param  dword ip [in] 登録IPアドレス
    \param  char *mac [in] 登録MACアドレス
    \return void 無し 
        
    \author Yamami
    \date   create:2004/09/19 update:$Date$
*/
void MoArp::addArpCache(dword ip, byte *mac)
{

    char IpKey[10];    //IPアドレスキー
    
    byte* setMac;
    
    setMac = new byte(6);
    memcpy(setMac,mac,6);

    //IPアドレスを、MAPのキー化(String化)
    sprintf(IpKey , "%08x",ip);
    ArpCache->put(IpKey , setMac);

}


/*!
    \brief searchCache
         ARPキャッシュ検索処理
    \param  dword ip [in] 登録IPアドレス
    \return char *mac [in] 検索結果MACアドレス 見つからない場合はNULL
        
    \author Yamami
    \date   create:2004/09/19 update:$Date$
*/
byte* MoArp::searchCache(dword ip)
{
    char IpKey[10];    //IPアドレスキー
    byte *RetMacValue;
    
    //HashMapから検索
    //IPアドレスを、MAPのキー化(String化)
    sprintf(IpKey , "%08x",ip);
    RetMacValue = ArpCache->get(IpKey);

    return RetMacValue;
}




/*!
    \brief getMac
         ARP IPアドレス解決処理
    \param  dword ip [in] IPアドレス
    \param  byte *mac [OUT] 解決MACアドレスへのポインタ
    \return int 0:キャッシュヒット 0以外:ARP要求をかけた、その待ち番号
        
    \author Yamami
    \date   create:2004/09/20 update:$Date$
*/
int MoArp::getMac(dword ip,byte *mac)
{
    MAC_REPLY_WAIT* nowWait;
    int waitNumber;
    
    enum{
        RETRY_COUNT=3,          // リトライ送信回数
        REQUEST_TIMEOUT=2000,   // リクエストタイムアウトミリ秒
    };

    byte *retmac;
    

    // キャッシュを検索
    if((retmac=searchCache(ip))!=NULL)
    {
        //キャッシュから見つかれば、返す。
        memcpy(mac,retmac,6);
        return 0;
    }


    //キャッシュに無いのでARP要求

    // 送信準備 待ちリストに登録
    nowWait = new MAC_REPLY_WAIT();
    
    nowWait->ip=ip;
    nowWait->repFlg=0;
    nowWait->wait=0;

    //待ちリストに追加
    macWaitList->add(nowWait);
    //待ち番号の取得
    //TO DO Yamami!!! ここスレッドセーフにしないとやばそう・・・
    waitNumber = macWaitList->size();

    // リクエストARPを送信
    //ブロードキャストで、ARP要求
    transArp(ip,broadcastMac,OPE_CODE_ARP_REQ);
    
    // 待ち番号を返却
    return waitNumber;

}

