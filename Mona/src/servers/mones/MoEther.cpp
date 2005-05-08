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
#include "MoEther.h"


/*!
    \brief initialize
         MoEther コンストラクタ
    \author Yamami
    \date   create:2004/08/12 update:
*/
MoEther::MoEther()
{
    //printf("MoEther Constructor\n");
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
    Ether_FrameList_ = new HList<ETHER_FRAME*>();

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
    delete Ether_FrameList_;
}


/*!
    \brief setEtherFrame
         イーサネットフレームセット

    \param  byte *frameBuf [in] イーサネットフレームバッファ
    \param  int size [in] イーサネットフレームサイズ
    \return int 結果 
        
    \author Yamami
    \date   create:2004/08/12 update:
*/
int MoEther::setEtherFrame(byte *frameBuf, int size)
{
    
    // allocate ETHER_FRAME
    ETHER_FRAME* frame = (ETHER_FRAME*)malloc(sizeof(ETHER_FRAME));
    // フレームのコピー
    memcpy(frame , frameBuf , size);

    //ここで、パケットユーティリティクラスを使って、エンディアン変換
    //フレームタイプ
    frame->type = MoPacUtl::packet_get_2byte(frameBuf , 12);

//2005/05/06 フレームタイプを表示
//printf("frame->type:%x \n",frame->type);
    
    //リストに追加
    Ether_FrameList_->add(frame);

    return 0;

}


/*!
    \brief getEtherFrame
         イーサネットフレーム取得 
    \param  byte* frameBuf [in] イーサネットフレームバッファへのポインタ
    \return int 結果 
        
    \author Yamami
    \date   create:2004/08/12 update:
*/
int MoEther::getEtherFrame(ETHER_FRAME *frameBuf)
{
    

    ETHER_FRAME* tempEther = Ether_FrameList_->removeAt(Ether_FrameList_->size() - 1);

    if (tempEther == NULL) {
        printf("tmp Null!!\n");
        return 0;
    }

    /* copy to keyinfo */
    memcpy(frameBuf, tempEther, sizeof(ETHER_FRAME));
    
    free(tempEther);
    
    return 1;

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
    int reti;
    
    // allocate ETHER_FRAME
    ETHER_FRAME* frame = (ETHER_FRAME*)malloc(sizeof(ETHER_FRAME));
    
    reti = getEtherFrame(frame);

    //Yamamiデバッグ
    //int i;
    //for(i=0;i<60;i++){
    //    printf("%d:%x ",i,frame->data[i]);
    //}
    //printf("\n");

    // フレームを各プロトコルに渡す
    switch(frame->type)
    {
        case ETHER_PROTO_IP:
            return g_MoIp->receiveIp((IP_HEADER*)frame->data);
            break;
        case ETHER_PROTO_ARP:
            return g_MoArp->receiveArp((ARP_HEADER*)frame->data);
            break;
            
    }
    
    return 0;
}




/*!
    \brief receiveEther
         イーサネットフレーム受信処理 
         バッファリングせず、直接処理版
    \param  byte* frameBuf [in] イーサネットフレームバッファへのポインタ
    \return int 結果 
    \author Yamami
    \date   create:2004/11/15 update:$Date$
*/
//int MoEther::receiveEther(ETHER_FRAME *frameBuf)
//{
//    
//    // フレームを各プロトコルに渡す
//    switch(frameBuf->type)
//    {
//        case ETHER_PROTO_IP:
//            return g_MoIp->receiveIp((IP_HEADER*)frameBuf->data);
//            break;
//        case ETHER_PROTO_ARP:
//            return g_MoArp->receiveArp((ARP_HEADER*)frameBuf->data);
//            break;
//            
//    }
//    
//    return 0;
//}



/*!
    \brief sendEther
         イーサネットフレーム送信処理 
    \param  byte *pkt [in] 送信パケットへのポインタ
    \param  dword dest_ip [in] 送信先IPアドレス
    \param  dword size [in] 送信サイズ
    \return int 結果 
        
    \author Yamami
    \date   create:2004/08/28 update:
*/
void MoEther::sendEther(byte *pkt , dword dest_ip , dword size)
{

    //Yamami?? 未実装 送信は、アプリ側からコールされるはず 送受信でクラスは分けるべきか？
    //byte   *mac;

    /* ARP テーブル検索 */
    //mac=arp_lookup(dest_ip);
    
    //送信
    //frame_output( pkt, mac, size, ETHER_PROTO_IP );
}

