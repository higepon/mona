/*!
    \file   MoIp.cpp
    \brief  IPプロトコル実装クラス

    Copyright (c) 2004 Yamami
    All rights reserved.
    License=MIT/X License

    \author  Yamami
    \version $Revision$
    \date   create:2004/09/20 update:$Date$
*/

/*! \class MoIp
 *  \brief IPプロトコル実装クラス
 */



#include "MoIp.h"
#include "MonesConfig.h"
#include "MonesGlobal.h"

/*!
    \brief initialize
         MoIp コンストラクタ
    \author Yamami
    \date   create:2004/09/20 update:
*/
MoIp::MoIp()
{

}

/*!
    \brief initialize
         MoIp initIp
    \author Yamami
    \param  AbstractMonic *pminsNic [in] NICクラスへのポインタ
    \date   create:2004/09/20 update:
*/
void MoIp::initIp(AbstractMonic *pminsNic ) 
{
    //NICクラスを保持
    insAbstractNic = pminsNic;
    return;
}


/*!
    \brief initialize
         MoIp デスクトラクタ
    \author Yamami
    \date   create:2004/08/20 update:
*/
MoIp::~MoIp() 
{

}


/*!
    \brief receiveIp
         IPプロトコル受信 処理
    \param  IP_HEADER *ipHead [in] IPヘッダへのポインタ
    \return int 結果 
        
    \author Yamami
    \date   create:2004/09/20 update:2004/09/20
*/
int MoIp::receiveIp(IP_HEADER *ipHead)
{

    /* チェックサムの確認。 */
    if(MoPacUtl::calcCheckSum((dword*)ipHead,(ipHead->verhead&0xf)*4)){
        return 0;
    }

    /* 上位プロトコルへ渡す。 */
    switch(ipHead->prot)
    {
        //case IPPROTO_TCP:
        //  return g_MoIcmp->receiveTcp(ipHead);
        //case IPPROTO_UDP:
        //  return g_MoIcmp->receiveUdp(ipHead);
        case IPPROTO_ICMP:
          return g_MoIcmp->receiveIcmp(ipHead);
    }

    return 0;
}


/*!
    \brief transIp
         IPプロトコル送信 処理
    \param  TRANS_BUF_INFO *tbi [in] 送信バッファ構造体へのポインタ
    \param  dword dstip [in] 送信先IPアドレス
    \param  byte tos [in] サービスタイプ
    \param  int flag [in] フラグ
    \return int 結果 

    \author Yamami
    \date   create:2004/09/20 update:2004/09/20
*/
int MoIp::transIp(TRANS_BUF_INFO *tbi, dword dstip, byte tos, int flag)
{

    byte *transPacket; //パケット送信バッファ

    char dstmac[6];
    dword transip;
    //int num;
    int rest;
    IP_HEADER ipHead;
    int max,total;
    //int trans;

    //Yamami ここでルーティング解決？？ とりあえず、dst = transとする。
    transip = dstip;

    // 送信先 MACアドレス取得(ARP解決)
    if((rest=g_MoArp->getMac(transip,dstmac))<0){
        return rest;
    }

    //IPヘッダを作成する。
    ipHead.verhead=IP_HEAD_VERSION|(sizeof(IP_HEADER)/4);
    ipHead.tos=tos;
    
    //GET_ID(ipHead.id);
    //Yamami ここはスレッドセーフにする必要がある？
    ipHead.id++;
    
    ipHead.ttl=255;
    ipHead.prot=tbi->ipType;
    ipHead.srcip=MoPacUtl::swapLong(G_MonesCon.getGl_myIpAdr());
    ipHead.dstip=dstip;
    ipHead.chksum=0;

    //送信TBL
    tbi->type=ETHER_PROTO_IP;
    tbi->data[0]=(char*)&ipHead;
    tbi->size[0]=sizeof(IP_HEADER);         // IP header size.
    //max=ethDev[num].mtu-sizeof(IP_HEADER);  // データ最大送信サイズ。
    max=G_MonesCon.getGl_myMTU()-sizeof(IP_HEADER);  // データ最大送信サイズ。
    total=tbi->size[1]+tbi->size[2];        // データトータルサイズ。

    if(total<=max)
    {
        
        
        //送信要求パケットサイズがMTU以下ならば
        //IPサイズ
        ipHead.len=MoPacUtl::swapShort(tbi->size[0]+total);
        //フラグ
        ipHead.frag=MoPacUtl::swapShort(flag<<14);
        //チェックサム
        ipHead.chksum=MoPacUtl::calcCheckSum((dword*)&ipHead,tbi->size[0]);
        
        //抽象NICを使って送信!
        //ethDev[num].dev->write(ethDev[num].dev->linf,(size_t)tbi,(size_t)dstmac);
        //memcpy(ipHead.dstMac,tbi->data[1],tbi->size[1]);
        
        //パケット作成
        transPacket = (byte *)malloc(sizeof(IP_HEADER) + total);
        memcpy(transPacket,(byte *)&ipHead,sizeof(ipHead));
        memcpy(transPacket + sizeof(ipHead),tbi->data[1],tbi->size[1]);
        
        insAbstractNic->frame_output(transPacket , (byte *)dstmac , sizeof(IP_HEADER) + total , ETHER_PROTO_IP);
        
        //パケットバッファ解放
        free(transPacket);
    }

/*
    else
    {
        //送信要求パケットサイズがMTU以上ならば  未実装
        ipHead.len=swapWord(tbi->size[0]+max);
        ipHead.frag=swapWord(IP_HEAD_FRAG_ON);
        tbi->size[2]=max-tbi->size[1];
        ipHead.chksum=calcSum((uint*)&ipHead,tbi->size[0]);
        ethDev[num].dev->write(ethDev[num].dev->linf,(size_t)tbi,(size_t)dstmac);

        trans=max;                      // 送信済サイズ
        tbi->data[2]+=max-tbi->size[1];
        tbi->size[1]=0;

        for(;;)
        {
            ipHead.chksum=0;

            if(total-trans<=max)
            {
                ipHead.len=swapWord(tbi->size[0]+total-trans);
                ipHead.frag=swapWord(trans);
                GET_ID(ipHead.id);
                ipHead.chksum=calcSum((uint*)&ipHead,tbi->size[0]);
                tbi->size[2]=total-trans;
                ethDev[num].dev->write(ethDev[num].dev->linf,(size_t)tbi,(size_t)dstmac);
                break;
            }
            else
            {
                ipHead.len=swapWord(tbi->size[0]+max);
                ipHead.frag=swapWord(trans|IP_HEAD_FRAG_ON);
                GET_ID(ipHead.id);
                ipHead.chksum=calcSum((uint*)&ipHead,tbi->size[0]);
                tbi->size[2]=max;
                ethDev[num].dev->write(ethDev[num].dev->linf,(size_t)tbi,(size_t)dstmac);
            }

            trans+=max;
            tbi->data[2]+=max;
        }
    }
*/

    return 0;
}


 
/*!
    \brief ipRouting
         IPルーティング 処理
    \param  word ip [in] 送信先IPアドレス
    \param  dword dstip [in] 転送先IPアドレス(他のサブネットならルータとなる)
    \return int 結果 

    \author Yamami
    \date   create:2004/10/30 update:2004/10/30
*/
int MoIp::ipRouting(dword ip,dword *transip)
{
    //int i;

    //同一サブネット内かチェックする。
    if((ip & G_MonesCon.getGl_mySubnet()) == (G_MonesCon.getGl_myIpAdr() & G_MonesCon.getGl_mySubnet()))
    {
        *transip=ip;
        return 0;
    }

    /* デフォルトゲートウェイへ。 */
    *transip=G_MonesCon.getGl_myGw();
    return 0;
}
