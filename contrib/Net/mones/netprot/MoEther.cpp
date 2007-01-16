/*!
    \file   MoEther.cpp
    \brief  イーサネット処理クラス

    Copyright (c) 2004 Yamami
    All rights reserved.
    License=MIT/X License

    \author  Yamami
    \version $Revision$
    \date   create:2004/08/12 update:$Date$
*/

/*! \class MoEther
 *  \brief イーサネット処理クラス
 */


#include "MonesGlobal.h"



/*!
    \brief initialize
         MoEther コンストラクタ
    \author Yamami
    \date   create:2004/08/12 update:
*/
MoEther::MoEther()
{
    
}

/*!
    \brief initialize
         MoEther etherInit
    \author Yamami
    \param  AbstractMonic *pminsNic [in] ドライバクラスへのポインタ(本来は、抽象クラス)
    \date   create:2004/08/12 update:
*/
void MoEther::etherInit(AbstractMonic *pminsNic) 
{

    
    //イーサネットフレーム保持リスト 生成
    /* keyinfo list */
    //Ether_FrameList_ = new HList<ETHER_FRAME*>();

    //内部保持のNICドライバへ格納
    insAbstractNic = pminsNic;

    return;
}


/*!
    \brief initialize
         MoEther デスクトラクタ
    \author Yamami
    \date   create:2004/08/12 update:
*/
MoEther::~MoEther() 
{
    //delete Ether_FrameList_;
}


/*!
    \brief setEtherFrame
         イーサネットフレームセット

    \param  uint8_t *frameBuf [in] イーサネットフレームバッファ
    \param  int size [in] イーサネットフレームサイズ
    \return int 結果 
        
    \author Yamami
    \date   create:2004/08/12 update:
*/
int MoEther::setEtherFrame(uint8_t *frameBuf, int size)
{
    
    // allocate ETHER_FRAME
    //ETHER_FRAME* frame = (ETHER_FRAME*)malloc(sizeof(ETHER_FRAME));
    
    //フレームバッファのクリア
    memset(&Ether_FrameBuf_ , 0 ,sizeof(Ether_FrameBuf_));
    
    // フレームのコピー
    memcpy(&Ether_FrameBuf_ , frameBuf , size);

    //ここで、パケットユーティリティクラスを使って、エンディアン変換
    //フレームタイプ
    Ether_FrameBuf_.type = MoPacUtl::packet_get_2uint8_t(frameBuf , 12);

    return 0;

}



/*!
    \brief receiveEther
         イーサネットフレーム受信処理 
    \return int 結果 
        
    \author Yamami
    \date   create:2004/08/22 update:
*/
int MoEther::receiveEther()
{

    // フレームを各プロトコルに渡す
    switch(Ether_FrameBuf_.type)
    {
        case ETHER_PROTO_IP:
            return g_MoIp->receiveIp((IP_HEADER*)Ether_FrameBuf_.data);
            break;
        case ETHER_PROTO_ARP:
            return g_MoArp->receiveArp((ARP_HEADER*)Ether_FrameBuf_.data);
            break;
            
    }
    
    return 0;
}



/*!
    \brief sendEther
         イーサネットフレーム送信処理 
    \param  uint8_t *pkt [in] 送信パケットへのポインタ
    \param  uint32_t dest_ip [in] 送信先IPアドレス
    \param  uint32_t size [in] 送信サイズ
    \return int 結果 
        
    \author Yamami
    \date   create:2004/08/28 update:
*/
void MoEther::sendEther(uint8_t *pkt , uint32_t dest_ip , uint32_t size)
{

    //Yamami?? 未実装 送信は、アプリ側からコールされるはず 送受信でクラスは分けるべきか？
    //uint8_t   *mac;

    /* ARP テーブル検索 */
    //mac=arp_lookup(dest_ip);
    
    //送信
    //frame_output( pkt, mac, size, ETHER_PROTO_IP );
}

