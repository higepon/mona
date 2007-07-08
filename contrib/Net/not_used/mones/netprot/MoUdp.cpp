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
#include <netprot/SocketsManager.h>
#include <monesoc/SocketContainer.h>

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
    if(MoPacUtl::calcCheckSumDummyHead((uint32_t*)&dmyhead,(uint32_t*)udp,sizeof(DUMMY_HEADER),udp_size)){
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
    \param  SocketContainer* soc [in] 送信SocketContainer
    \return 無し
        
    \author Yamami
    \date   create:2005/08/09 update:
*/
void MoUdp::transUdp(SocketContainer* soc)
{
    
    TRANS_BUF_INFO tbi;
    UDP_HEADER* udpHead;
    DUMMY_HEADER dmyhead;

    //SocketContainerから、パケット(バイト列)を取り出す。
    packet_cmemoryinfo pcm;

logprintf("MoUdp::transUdp soc->getBuffer(1) Pre!!\n");
logprintf("soc Addr = %x \n",soc);
    
    pcm = soc->getBuffer(1);

logprintf("MoUdp::transUdp soc->getBuffer(1) After!!\n");

    //共有メモリからパケット バイト を取得                
    monapi_cmemoryinfo* cmPacByte;
    
    cmPacByte = monapi_cmemoryinfo_new();
    cmPacByte->Handle = pcm.Handle;
    cmPacByte->Size   = pcm.Size;
    monapi_cmemoryinfo_map(cmPacByte);                

    //UDP送信用のメモリ確保
    udpHead = (UDP_HEADER*)malloc(sizeof(UDP_HEADER) + cmPacByte->Size);
    
    int udp_size;
    udp_size = cmPacByte->Size + sizeof(UDP_HEADER);


logprintf("MoUdp::transUdp udp_size = %d\n",udp_size);
    
    //UDPヘッダーの設定
    udpHead->srcport = MoPacUtl::swapShort(soc->myport);
    udpHead->dstport = MoPacUtl::swapShort(soc->youport);
    udpHead->len = MoPacUtl::swapShort((short)udp_size);


logprintf("MoUdp::transUdp cmPacByte->Data = %s\n",cmPacByte->Data);

    
    //UDPのバッファへコピー
    memcpy(udpHead->data,cmPacByte->Data , cmPacByte->Size);                

    udpHead->chksum=0;
    // チェックサムの生成
    // UDPのチェックサムは、疑似ヘッダをつけて行う。
    dmyhead.srcip=MoPacUtl::swapLong(G_MonesCon.getGl_myIpAdr());
    dmyhead.dstip=MoPacUtl::swapLong(soc->ip);
    dmyhead.tmp=0;
    dmyhead.prot=IPPROTO_UDP;
    dmyhead.len=udpHead->len;
    udpHead->chksum = MoPacUtl::calcCheckSumDummyHead((uint32_t*)&dmyhead,(uint32_t*)udpHead,sizeof(DUMMY_HEADER),udp_size);

    //送信バッファテーブルの設定
    tbi.data[2]=NULL;
    tbi.size[2]=0;
    tbi.data[1]=(char*)udpHead;
    tbi.size[1]=udp_size;
    tbi.ipType=IPPROTO_UDP;

    //共有メモリ解放
    monapi_cmemoryinfo_dispose(cmPacByte);
    monapi_cmemoryinfo_delete(cmPacByte);                
    
    g_MoIp->transIp(&tbi,MoPacUtl::swapLong(soc->ip),0,0);
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
//    if (MoPacUtl::swapShort(udp->dstport) == 2600){
//        char buf[1530];
//        memset(buf,0,sizeof(buf));
//        logprintf("srcip = %x\n",MoPacUtl::swapLong(ipHead->srcip));
//        logprintf("dstport = %x\n",MoPacUtl::swapShort(udp->dstport));
//        logprintf("size = %x\n",udp_size);
//        memcpy(buf, udp->data , udp_size);
//        for(int i = 0; i < udp_size ; i++){
//            logprintf("%c",buf[i]);
//        }
//        logprintf("\n");
//    }

    //リスンあるいは、コネクトしているソケットがあれば
    if(SocketContainer* soc = g_SocketsManager->findSocket(MoPacUtl::swapLong(ipHead->srcip) 
        , MoPacUtl::swapShort(udp->dstport) 
        , MoPacUtl::swapShort(udp->srcport))){
        
        int reti;

        //ソケットの受信バッファへ追加
        reti = soc->addBuffer(udp->data , udp_size , 0);
        
        //登録しているプロセスに通知する。
        // create message
        //TODO このメッセージは不適切(ICMP) プロトコルごとにメッセージ変える？
        Message::create(&info, MSG_MONES_ICMP_NOTICE, 0, 0, 0, NULL);
        
        //メッセージにSocketContainerをセット
        //reti = MoPacUtl::createPacMsg(&info , soc);
        reti = soc->createPacMonaMsg(&info , IPPROTO_UDP);
        
        // send
        if (Message::send(soc->tid, &info)) {
            //printf("MoUdp::saveRecv error\n");
        }
        
    }
    
}
