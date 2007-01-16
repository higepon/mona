/*!
    \file   main.cpp
    \brief  Yamami Code:Mones ping もどき
    
    Copyright (c) 2005 Yamami
    All rights reserved.
    License=MIT/X License

    \author  Yamami
    \version $Revision$
    \date   create:2005/05/09 update:$Date$
*/

//※※ 2005/08/09現在。動かない!!! MonaMainの引数処理??

#include <sys/types.h>
#include <monapi.h>
#include <sys/HashMap.h>
#include <monalibc/stdio.h>

//IP関連のヘッダはどこに定義すべきか？
//とりあえず、Mones内のMoDefineを強引にinclude
#include "../../netprot/MonesDefine.h"


using namespace MonAPI;


//Code:Mones ICMP送信
int main(int argc, char* argv[])
{

    if(argc < 2)){
        printf("Arguments is few!\n");
        printf("usage: ping host\n");
        return 0;
    }
    
    
    printf("argv[1]=%s\n",pekoe->get(0));
    
    int a,b,c,d;
    
    sscanf(argv[1],"%d.%d.%d.%d",&a,&b,&c,&d);
    
    printf("a=%d\n",a);
    printf("b=%d\n",b);
    printf("c=%d\n",c);
    printf("d=%d\n",d);

    //IPアドレス組み立て
    uint32_t ipaddr;
    ipaddr = 0;
    ipaddr = (uint8_t)a;
    ipaddr = (ipaddr << 8) + (uint8_t)b;
    ipaddr = (ipaddr << 8) + (uint8_t)c;
    ipaddr = (ipaddr << 8) + (uint8_t)d;
    
    printf("Pinding %s\n",argv[1]);

    //ここで、Monesにメッセージを送る
    MessageInfo info;

    uint32_t targetID = Message::lookupMainThread("MONES.EX5");
    if (targetID == 0xFFFFFFFF)
    {
        printf("MONES.EX5 not found\n");
        exit(1);
    }

    //Monesへ登録
    // create message
    Message::create(&info, MSG_MONES_REGIST, ipaddr, 0, 0, NULL);
    // send
    if (Message::send(targetID, &info)) {
        printf("MSG_MONES_REGIST error\n");
    }
    
    
    //IP送信
    // create message
    Message::create(&info, MSG_MONES_IP_SEND, 0, 0, 0, NULL);

    //ICMPパケットの作成
    int icmp_size;
    icmp_size=40;
    
    char icmp_buf[40];
    
    ICMP_HEADER *volatile icmpHead;
    icmpHead=(ICMP_HEADER*)icmp_buf;
    
    
    //dataの部分をつくる
    memcpy(icmpHead->data , "abcdefghijklmnopqrstuvwabcdefghi",32);
    
    //ICMPヘッダーの設定
    icmpHead->type=ICMP_TYPE_ECHOREQ;
    icmpHead->code=0;
    //チェックサムはここでは計算しない。Monesに任せる
    //icmpHead->chksum=0;
    //icmpHead->chksum=MoPacUtl::calcCheckSum((uint32_t*)icmpHead,icmp_size);

    info.arg1 = 1;  //IPPROTO_ICMP
    info.arg2 = ipaddr;
    
    memcpy(info.str , icmp_buf,icmp_size);
    info.length = icmp_size;

    // send
    if (Message::send(targetID, &info)) {
        printf("MSG_MONES_IP_SEND error\n");
    }
    
    
    /* Message loop */
    //ここでメッセージループして、応答を待ってみる。
    for (;;)
    {
        /* receive */
        if (!Message::receive(&info))
        {
            
            switch(info.header)
            {
            case MSG_MONES_ICMP_NOTICE:
                //IP要求が返ってきたら
                printf("Reply from %s\n",argv[1]);
                for(int i = 0 ; i < info.length ; i++){
                    printf("%x ",info.str[i]);
                }
                
                return 0;
                
                break;

            default:
                /* igonore this message */
                break;
            }

        }
    }
    
    return 0;

}

