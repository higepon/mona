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
    //printf("MoArp Constructor\n");
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

//
//    Yamamiデバッグ
//    char logStr[1024];    //ログ出力文字列
//
//    sprintf(logStr , "swapLong(G_MonesCon.getGl_myIpAdr):%04x dstIp:%04x \n",
//                MoPacUtl::swapLong(G_MonesCon.getGl_myIpAdr),arpHead->dstIp);
//    sprintf(logStr , "swapLong(G_MonesCon.getGl_myIpAdr):%04x dstIp:%04x \n",
//                MoPacUtl::swapLong(G_MonesCon.getGl_myIpAdr),arpHead->dstIp);
//    sprintf(logStr , "G_MonesCon.getGl_myIpAdr:%04x \n",
//                G_MonesCon.getGl_myIpAdr);
//    sprintf(logStr , "G_MonesCon.getGl_myIpAdr:%04x \n",
//                G_MonesCon.getGl_myIpAdr);
//    sprintf(logStr , "hardType:%02x \n",
//                arpHead->hardType);
//    printf(logStr);
//    
//    sprintf(logStr , "protType:%02x \n",
//                arpHead->protType);
//    printf(logStr);
//
//    sprintf(logStr , "hardAddrLen:%01x \n",
//                arpHead->hardAddrLen);
//    printf(logStr);
//    
//    sprintf(logStr , "protAddrLen:%01x \n",
//                arpHead->protAddrLen);
//    printf(logStr);
//
//    sprintf(logStr , "opeCode:%02x \n",
//                arpHead->opeCode);
//    printf(logStr);
//
//    sprintf(logStr , "srcMac:%02x:%02x:%02x:%02x:%02x:%02x \n",
//                arpHead->srcMac[0],
//                arpHead->srcMac[1],
//                arpHead->srcMac[2],
//                arpHead->srcMac[3],
//                arpHead->srcMac[4],
//                arpHead->srcMac[5]);
//    printf(logStr);
//
//    sprintf(logStr , "srcIp:%04x \n",
//                arpHead->srcIp);
//    printf(logStr);
//
//    sprintf(logStr , "dstMac:%02x:%02x:%02x:%02x:%02x:%02x \n",
//                arpHead->dstMac[0],
//                arpHead->dstMac[1],
//                arpHead->dstMac[2],
//                arpHead->dstMac[3],
//                arpHead->dstMac[4],
//                arpHead->dstMac[5]);
//    printf(logStr);
//
//    sprintf(logStr , "dstIp:%04x \n",
//                arpHead->dstIp);
//    printf(logStr);



    // オペレーションコードの確認
    switch(MoPacUtl::swapShort(arpHead->opeCode))
    {
        //ARP要求
        case OPE_CODE_ARP_REQ:
            transArp(arpHead->srcIp,arpHead->srcMac, OPE_CODE_ARP_REP);
            
            //キャッシュに登録 2004/08/28未実装
            //addCache(arpHead->srcIp,arpHead->srcMac); 
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
