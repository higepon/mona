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

            //Yamami!! TO DO パケット送信処理、未実装
            //アプリからのパケット送信要求
            case MSG_MONES_IP_SEND:
                //パケット送信
                //TO DO 単純に、ここでテスト用のIPパケットを作成し送信要求
                printf("MSG_MONES_IP_SEND\n");
                
                int ret;
                dword ip;
                TRANS_BUF_INFO tbi;
                char* testPacket;
                
                testPacket = new char(100);
                
                //送信先 10.0.2.2(QEMU GW)
                ip = 0x0A000202;
                
                sprintf(testPacket , "TEST_PACKET!!!!");
                
                
                //送信バッファテーブルの設定
                tbi.data[2]=NULL;
                tbi.size[2]=0;
                tbi.data[1]=(char*)testPacket;
                tbi.size[1]=1024;
                tbi.ipType=IPPROTO_IP;
                
                ret = g_MoIp->transIp(&tbi , MoPacUtl::swapLong(ip) ,0, 0);
                
                break;

            //ARP待ちからのWake Up
            case MSG_MONES_WAKEUP_ARP_WAIT:
                
                printf("MSG_MONES_WAKEUP_ARP_WAIT\n");
                
                MAC_REPLY_WAIT* nowWait;
                TRANS_BUF_INFO *ipsend;
    
                //ARP要求待ちリストの検索
                
                //Yamami デバッグ
                printf("Yamami!!!\n");
                printf("g_MoArp->macWaitList->size() = %d \n",g_MoArp->macWaitList->size());
                
                
                for (int i = 0; i < g_MoArp->macWaitList->size() ; i++) {
                    nowWait = g_MoArp->macWaitList->get(i);
                    
                    if(nowWait->repFlg == 1){
                        //ARP解決済みなら、待ちパケットを送信する。
                        ret = g_MoIp->transIp(nowWait->ipPacketBuf , nowWait->ip ,0, 0);
                        
                        //待避していたIPパケットバッファの解放
                        //一旦コメント化
                        //free(nowWait->ipPacketBuf);
                        
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

