/*!
  \file  mones.cpp
  \brief Code:Monesメイン

  Copyright (c) 2004 Yamami
  All rights reserved.
  License=MIT/X Licnese

  \author  Yamami
  \version $Revision$
  \date   create:2004/08/08 update:$Date$
*/


//ここで、Monesのグローバルオブジェクトをインスタンス化する
#define MONES_GLOBAL_VALUE_DEFINED
#include "MonesGlobal.h"

#include <monapi.h>
#include "AbstractMonic.h"
#include "MoEther.h"
#include "MoIcmp.h"
#include "MonesConfig.h"
#include "MonesLoader.h"



using namespace MonAPI;


/*!
    \brief MonaMain
        Code:Monesメイン

    \param List<char*>* pekoe

    \author  Yamami
    \date    create:2004/08/08 update:2004/10/31
*/
int MonaMain(List<char*>* pekoe)
{
    
    //TO DO とりあえず、30プロセスまでOK 
    MonesRList = new HList<MONES_IP_REGIST*>();
    MONES_IP_REGIST *regist;
    
    //syscall_get_io  
    //このプロセス動作中は、IOを特権レベル3まで許可する
    syscall_get_io();
    
    // NICのインスタンス化
    // MonesLoaderクラス経由でインスタンス化する。
    AbstractMonic* insAbstractNic;
    
    MonesLoader* insNicLoader = new MonesLoader();
    insNicLoader->setup();
    insAbstractNic = insNicLoader->getNicInstance();
    //NICのロードに失敗した場合は、Mones終了
    if(insAbstractNic == 0){
        printf("NIC Error Mones Quit \n");
        return 0;
    }
    
    //Etherクラスのインスタンス化
    g_MoEther = new MoEther();
    g_MoEther->etherInit(insAbstractNic);

    //ARPクラスのインスタンス化
    g_MoArp = new MoArp();
    g_MoArp->initArp(insAbstractNic);

    //IPクラスのインスタンス化
    g_MoIp = new MoIp();
    g_MoIp->initIp(insAbstractNic);


    // initilize destination list
    //List<dword>* destList = new HList<dword>();
    MessageInfo info;

    // Server start ok
    dword targetID = Message::lookupMainThread("MONITOR.BIN");
    if (targetID == 0xFFFFFFFF)
    {
        printf("Mones:INIT not found\n");
        exit(1);
    }

    // create message
    Message::create(&info, MSG_SERVER_START_OK, 0, 0, 0, NULL);

    // send
    if (Message::send(targetID, &info)) {
        printf("Mones:INIT error\n");
    }

    //ネットワークのIRQマスクEnable
    insAbstractNic->enableNetWork();

    //IRQレシーバとして登録 (IRQは、NICドライバクラスより得る)
    syscall_set_irq_receiver(insAbstractNic->getNicIRQ());

    /* Message loop */
    //ここでメッセージループ
    for (;;)
    {
        /* receive */
        if (!Message::receive(&info))
        {
            
            switch(info.header)
            {
            case MSG_INTERRUPTED:
                
                //printf("MSG_INTERRUPTED\n");
                
                dword    i;
                
                //パケット受信
                insAbstractNic->frame_input();
                
                //Etherクラスに登録
                i = g_MoEther->setEtherFrame(insAbstractNic->frame_buf ,insAbstractNic->frame_len);
                //イーサネットフレーム受信処理
                i = g_MoEther->receiveEther();
                
                break;


            //Monesへ登録
            case MSG_MONES_REGIST:
            //case 5:
                
                printf("MSG_MONES_REGIST\n");
                
                //通信管理リストに登録
                //TO DO TCPをサポートすれば、本来は、IPだけでは無く、ポート番号も
                regist = new MONES_IP_REGIST();
                regist->tid = info.from;
                regist->ip = info.arg1;
                
                MonesRList->add(regist);
                
                break;


            //Yamami!! TO DO パケット送信処理、未実装
            //アプリからのパケット送信要求
            case MSG_MONES_IP_SEND:
                //パケット送信
                //TO DO 単純に、ここでテスト用のIPパケットを作成し送信要求
                printf("MSG_MONES_IP_SEND\n");
                
                int ret;
                dword ip;
                TRANS_BUF_INFO tbi;
                
                ICMP_HEADER *icmpHead;
                icmpHead = new ICMP_HEADER();
                
                //testPacket = new char(100);
                
                //送信先 10.0.2.2(QEMU GW)
                ip = 0x0A000202;
                
                
                //ICMPヘッダーの設定
                icmpHead->type=ICMP_TYPE_ECHOREQ;
                icmpHead->code=0;
                icmpHead->chksum=0;
                //icmpHead->chksum=MoPacUtl::calcCheckSum((dword*)icmpHead,size);
                icmpHead->chksum=MoPacUtl::calcCheckSum((dword*)icmpHead,0);

                //送信バッファテーブルの設定
                tbi.data[2]=NULL;
                tbi.size[2]=0;
                tbi.data[1]=(char*)icmpHead;
                //tbi.size[1]=size;
                tbi.size[1]=0;
                tbi.ipType=IPPROTO_ICMP;
    
                ret = g_MoIp->transIp(&tbi , MoPacUtl::swapLong(ip) ,0, 0);

                //delete icmpHead;
                
                break;

            //ARP待ちからのWake Up
            case MSG_MONES_WAKEUP_ARP_WAIT:
                
                printf("MSG_MONES_WAKEUP_ARP_WAIT\n");
                
                MAC_REPLY_WAIT* nowWait;
                
                //ARP要求待ちリストの検索
                for (int i = 0; i < g_MoArp->macWaitList->size() ; i++) {
                    nowWait = g_MoArp->macWaitList->get(i);
                    
                    if(nowWait->repFlg == 1){
                        //ARP解決済みなら、待ちパケットを送信する。
                        ret = g_MoIp->transIp(nowWait->ipPacketBuf , nowWait->ip ,0, 0);
                        
                        //待避していたIPパケットバッファの解放
                        free(nowWait->ipPacketBuf);
                        
                        //待ちリストから削除
                        g_MoArp->macWaitList->removeAt(i);
                        //カウンタデクリメントがいる。
                        i--;
                    }
                }
                

                
                break;

            default:
                /* igonore this message */
                break;
            }

        }
    }
    return 0;
}

