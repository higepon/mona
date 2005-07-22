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
#include <monesoc/SocketsManager.h>
#include <monesoc/Socket.h>

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
    DUMMY_HEADER dmyhead;
    
    int udp_size;
    UDP_HEADER *udp;

    udp=(UDP_HEADER*)ipHead->data;

    //udp_size=MoPacUtl::swapShort(ipHead->len)-sizeof(IP_HEADER);
    udp_size=MoPacUtl::swapShort(udp->len);

    // チェックサムの確認
    // UDPのチェックサムは、疑似ヘッダをつけて行う。
    dmyhead.srcip=ipHead->srcip;
    dmyhead.dstip=ipHead->dstip;
    dmyhead.tmp=0;
    dmyhead.prot=ipHead->prot;
    dmyhead.len=udp->len;
    if(MoPacUtl::calcCheckSumDummyHead((dword*)&dmyhead,(dword*)udp,sizeof(DUMMY_HEADER),udp_size)){
        logprintf("udp_size = %d\n",udp_size);
        logprintf("udp->chksum = %x\n",MoPacUtl::swapShort(udp->chksum));
        logprintf("udp CheckSum BAD!!\n");
        return 0;
    }

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

    int udp_size;
    UDP_HEADER *udp;

    udp=(UDP_HEADER*)ipHead->data;
    udp_size=MoPacUtl::swapShort(ipHead->len)-sizeof(IP_HEADER);

    //ここで到着UDPパケットをデバッグ
    if (MoPacUtl::swapShort(udp->dstport) == 2600){
        char buf[1530];
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

    //リスンあるいは、コネクトしているソケットがあれば
    if(Socket* soc = g_SocketsManager->findSocket(MoPacUtl::swapLong(ipHead->srcip) 
        , MoPacUtl::swapShort(udp->dstport) 
        , MoPacUtl::swapShort(udp->srcport))){
        
        int reti;
        
        
logprintf("if(Socket* soc = IN!!! \n");
        //ソケットの受信バッファへ追加
        reti = soc->addBuffer(udp->data , udp_size , 0);
        
logprintf("reti = soc->addBuffer After!!! \n");
        
        //登録しているプロセスに通知する。
        // create message
        //TODO このメッセージは不適切(ICMP) プロトコルごとにメッセージ変える？
        Message::create(&info, MSG_MONES_ICMP_NOTICE, 0, 0, 0, NULL);
        
        //MoPacUtlでメッセージにSocketをセット
        reti = MoPacUtl::createPacMsg(&info , soc);
        
        // send
        if (Message::send(soc->tid, &info)) {
            //printf("MoUdp::saveRecv error\n");
        }        
        
    }
    
}
