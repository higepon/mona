/*!
    \file   MoUdp.cpp
    \brief  UDPプロトコル実装クラス

    Copyright (c) 2005 Yamami
    All rights reserved.
    License=MIT/X License

    \author  Yamami
    \version $Revision$
    \date   create:2005/05/11 update:$Date$
*/

/*! \class MoUdp
 *  \brief UDPプロトコル実装クラス
 */



#include "MoUdp.h"
#include "MonesConfig.h"
#include "MonesGlobal.h"

/*!
    \brief initialize
         MoUdp コンストラクタ
    \author Yamami
    \date   create:2004/09/20 update:
*/
MoUdp::MoUdp()
{

}

/*!
    \brief initialize
         MoUdp initIp
    \author Yamami
    \param  AbstractMonic *pminsNic [in] NICクラスへのポインタ
    \date   create:2004/09/20 update:
*/
void MoUdp::initUdp(AbstractMonic *pminsNic ) 
{
    //NICクラスを保持
    insAbstractNic = pminsNic;
    return;
}


/*!
    \brief initialize
         MoUdp デスクトラクタ
    \author Yamami
    \date   create:2004/08/20 update:
*/
MoUdp::~MoUdp() 
{

}


/*!
    \brief receiveUdp
         UDPプロトコル受信 処理
    \param  IP_HEADER *ipHead [in] IPヘッダへのポインタ
    \return int 結果 
        
    \author Yamami
    \date   create:2004/09/20 update:2004/09/20
*/
int MoUdp::receiveUdp(IP_HEADER *ipHead)
{

    int udp_size;
    UDP_HEADER *udp;

    udp=(UDP_HEADER*)ipHead->data;

    //udp_size=MoPacUtl::swapShort(ipHead->len)-sizeof(IP_HEADER);
    udp_size=MoPacUtl::swapShort(udp->len);

    // チェックサムの確認
    //TODO 面倒なので後回し。この計算じゃダメ
    //if(MoPacUtl::calcCheckSum((dword*)udp,udp_size)){
    //    logprintf("udp_size = %d\n",udp_size);
    //    logprintf("udp->chksum = %x\n",MoPacUtl::swapShort(udp->chksum));
    //    logprintf("udp CheckSum BAD!!\n");
    //    return 0;
    //}

    saveRecv(ipHead,udp_size+sizeof(IP_HEADER));

    return 0;
}


/*!
    \brief transUdp
         UDP送信 処理
    \param  word dstip [in] 送信先IPアドレス
    \param  byte type [in] UDPタイプ
    \param  UDP_HEADER *udpHead [in] UDPヘッダへのポインタ
    \param  int size [in] パケットサイズ
    \return 無し
        
    \author Yamami
    \date   create:2004/09/20 update:2004/09/20
*/
void MoUdp::transUdp(dword dstip, byte type, byte code, UDP_HEADER *udpHead, int size)
{
    
    TRANS_BUF_INFO tbi;

    //UDPヘッダーの設定
    //udpHead->type=type;
    //udpHead->code=code;
    //udpHead->chksum=0;
    //udpHead->chksum=MoPacUtl::calcCheckSum((dword*)udpHead,size);

    //送信バッファテーブルの設定
    //tbi.data[2]=NULL;
    //tbi.size[2]=0;
    //tbi.data[1]=(char*)udpHead;
    //tbi.size[1]=size;
    //tbi.ipType=IPPROTO_UDP;

    g_MoIp->transIp(&tbi,dstip,0,0);
}


/*!
    \brief saveRecv
         UDP応答受信 処理
    \param  IP_HEADER *ipHead [in] IPヘッダ
    \param  int size [in] パケットサイズ
    \return 無し
    \author Yamami
    \date   create:2004/09/20 update:2004/09/20
*/
void MoUdp::saveRecv(IP_HEADER *ipHead, int size)
{
    MessageInfo info;
    MONES_IP_REGIST *regist;

    int udp_size;
    UDP_HEADER *udp;

    udp=(UDP_HEADER*)ipHead->data;
    udp_size=MoPacUtl::swapShort(ipHead->len)-sizeof(IP_HEADER);

    //ここで到着UDPパケットをデバッグ
    if (MoPacUtl::swapShort(udp->dstport) == 2600){
        char buf[128];
        memset(buf,0,sizeof(buf));
        logprintf("srcip = %x\n",MoPacUtl::swapLong(ipHead->srcip));
        logprintf("dstport = %x\n",MoPacUtl::swapShort(udp->dstport));
        logprintf("size = %x\n",udp_size);
        memcpy(buf, udp->data , udp_size);
        for(int i = 0; i < udp_size ; i++){
            logprintf("%c",buf[i]);
        }
        logprintf("\n");
    }

    //登録しているプロセスに通知する。
    for (int i = 0; i < MonesRList->size() ; i++) {
        regist = MonesRList->get(i);
        
        //UDPはコネクションレスなので、ポート番号のみ？
        if(regist->port == MoPacUtl::swapShort(udp->dstport)){
            //登録されているIPおよびポートへのUDPパケットならば、メッセージ通知
            // create message
            logprintf("Mones:MSG_MONES_ICMP_NOTICE send!!\n");
            Message::create(&info, MSG_MONES_ICMP_NOTICE, 0, 0, 0, NULL);
            memcpy(info.str , udp->data, udp_size);
            info.length = udp_size;
            // send
            if (Message::send(regist->tid, &info)) {
                //printf("MoUdp::saveRecv error\n");
            }
            break;
            
        }
    }
}
