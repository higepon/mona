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
    
    ArpCache = new HashMap<char*>(ARP_CACHE_NUM);
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
    \date   create:2004/08/28 update:2004/08/29
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
            
            //キャッシュに登録
            addArpCache(arpHead->srcIp,(char*)arpHead->srcMac); 
            break;
        //ARP応答
        case OPE_CODE_ARP_REP:
            //未実装
            //rest=receiveReply(arpHead->srcIp,arpHead->srcMac);
            break;
        
        //RARP要求
        case OPE_CODE_RARP_REQ:
        //RARP応答
        case OPE_CODE_RARP_REP:
            break;
    }

    return rest;

}



static char broadcastMac[]={0xff,0xff,0xff,0xff,0xff,0xff};

/*!
    \brief transArp
         ARP応答処理 処理
    \param  dword dstip [in] 送信先IPアドレス
    \param  byte *dstmac [in] 送信先MACアドレス
    \param  word opecode [in] オペレーションコード
    \return void 無し 
        
    \author Yamami
    \date   create:2004/08/28 update:
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
    insAbstractNic->frame_output((byte *)&head , dstmac , sizeof(head) , DIX_TYPE_ARP);

}


/*!
    \brief addArpCache
         ARPキャッシュ登録処理
    \param  dword ip [in] 登録IPアドレス
    \param  char *mac [in] 登録MACアドレス
    \return void 無し 
        
    \author Yamami
    \date   create:2004/09/19 update:
*/
void MoArp::addArpCache(dword ip, char *mac)
{

    char IpKey[10];    //IPアドレスキー

    //IPアドレスを、MAPのキー化(String化)
    sprintf(IpKey , "%08x",ip);
    ArpCache->put(IpKey , mac);

}


/*!
    \brief searchCache
         ARPキャッシュ検索処理
    \param  dword ip [in] 登録IPアドレス
    \return char *mac [in] 検索結果MACアドレス 見つからない場合はNULL
        
    \author Yamami
    \date   create:2004/09/19 update:
*/
char* MoArp::searchCache(dword ip)
{
    char IpKey[10];    //IPアドレスキー
    char *RetMacValue;
    
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
    \return void 無し 
        
    \author Yamami
    \date   create:2004/09/20 update:
*/
int MoArp::getMac(dword ip,char *mac)
{
    enum{
        RETRY_COUNT=3,          // リトライ送信回数
        REQUEST_TIMEOUT=2000,   // リクエストタイムアウトミリ秒
    };

    char *retmac;

    //int rest;
    //REPLY_WAIT *wait;
    //int i;
    //REPLY_WAIT *p;


    // キャッシュを検索
    if((retmac=searchCache(ip))!=NULL)
    {
        //キャッシュから見つかれば、返す。
        memcpy(mac,retmac,6);
        return 0;
    }

/*

    // 送信準備
    if((wait=kmalloc(sizeof(REPLY_WAIT)))==NULL)return -ENOMEM;
    wait->ip=ip;
    wait->repFlg=0;
    wait->next=replyWait;
    replyWait=wait;

    // リクエストARPを送信
    for(i=0;;)
    {
        transArp(num,ip,broadcastMac,OPE_CODE_ARP_REQ);
        wait->wait.flag=0;
        wait_intr(&wait->wait,REQUEST_TIMEOUT,TASK_SIGNAL_WAIT);
        if(wait->repFlg)
        {
            addCache(ip,wait->mac);
            memcpy(mac,wait->mac,6);
            rest=0;
            break;
        }
        if(++i>=RETRY_COUNT)
        {
            rest=-ENETUNREACH;
            break;
        }
    }

    // 送信後処理
    if(replyWait==wait)replyWait=wait->next;
    else
    {
        for(p=replyWait;p->next!=wait;p=p->next);
        p->next=wait->next;
    }
    kfree(wait);

    return rest;

*/

}

