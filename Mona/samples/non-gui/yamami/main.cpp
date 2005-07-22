/*!
    \file   main.cpp
    \brief  Yamami Code:Mones リリース前 各種テストロジック cvsadd
    
    cvs test

    Copyright (c) 2004 Yamami
    All rights reserved.
    License=MIT/X License

    \author  Yamami
    \version $Revision$
    \date   create:2004/08/28 update:$Date$
*/

#include <sys/types.h>
#include <monapi.h>
#include <pci/Pci.h>
#include <sys/HashMap.h>

//共有メモリ使用
#include <monapi/cmemoryinfo.h>

using namespace MonAPI;

#define MAIN_10






#ifdef MAIN_10

#include <monesoc/Socket.h>

//Code:Mones UDPライブラリ 実装前準備テスト
int MonaMain(List<char*>* pekoe)
{

    //ここで、Monesにメッセージを送る
    MessageInfo info;

    dword targetID = Message::lookupMainThread("MONES.EX5");
    if (targetID == 0xFFFFFFFF)
    {
        logprintf("MONES.EX5 not found\n");
        exit(1);
    }

    //Monesへ登録
    //ポート2600でリスン
    Message::create(&info, MSG_MONES_REGIST, 0, 2600, 0, NULL);
    // send
    if (Message::send(targetID, &info)) {
        logprintf("MSG_MONES_REGIST error\n");
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
                //返ってきたら
                logprintf("Reply \n");
                
                Socket *soc;
                
                //共有メモリ
                logprintf("use commonMem Handle = %d : Size = %d \n",info.arg2 , info.arg3);
                monapi_cmemoryinfo* cmSoc;

                cmSoc = monapi_cmemoryinfo_new();
                cmSoc->Handle = info.arg2;
                cmSoc->Size   = info.arg3;
                
                monapi_cmemoryinfo_map(cmSoc);
                
                //共有メモリからSocketを取り出す。
                //logprintf("Pre memcpy(soc,cmSoc->Data\n");
                //soc = new Socket();
                //memcpy(soc,cmSoc->Data , sizeof(Socket));
                //logprintf("After memcpy(soc,cmSoc->Data\n");
                
                soc = (Socket*)cmSoc->Data;

                //Socketから、パケット(バイト列)を取り出す。
                //UDPは、最大超えると読み捨てる。TCPはフロー制御が必要 UDPSocket とTCPSocketの仕事
                packet_cmemoryinfo pcm;
                
                pcm = soc->getBuffer(0);
                
                logprintf("pcm = soc->getBuffer(0);Call!! \n");
                
                logprintf("use Pcm Handle = %d : Size = %d \n",pcm.Handle , pcm.Size);
                
                //soc->recvBuff->removeAt(0);


                //共有メモリからパケット バイト を取得                
                monapi_cmemoryinfo* cmPacByte;
                
                cmPacByte = monapi_cmemoryinfo_new();
                cmPacByte->Handle = pcm.Handle;
                cmPacByte->Size   = pcm.Size;
                monapi_cmemoryinfo_map(cmPacByte);                

                char buff[1024];
                
                memcpy(buff,cmPacByte->Data , sizeof(buff));                
                
                printf("%s\n",buff);
                
                //Socketは、アプリで共有メモリ解放しない!!
                //monapi_cmemoryinfo_dispose(cmSoc);
                //monapi_cmemoryinfo_delete(cmSoc);                

                monapi_cmemoryinfo_dispose(cmPacByte);
                monapi_cmemoryinfo_delete(cmPacByte);                
                
                
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

#endif





#ifdef MAIN_9

class AlcTest
{
    public:
        int aaa;
        char bbb;
        byte mema[4096];
};



//メモリアロケータテスト
int MonaMain(List<char*>* pekoe)
{

    byte *mem;
    
    //malloc
    mem = (byte*)malloc(4096);
    
    //free
    free(mem);
    
    //new
    PciInf *pciinfo;
    //PCIライブラリクラスのインスタンス化
    AlcTest* alc = new AlcTest();
    
    delete alc;
    
    return 0;
}

#endif





#ifdef MAIN_1

//Yamami FileOutputStream使用テスト
int MonaMain(List<char*>* pekoe)
{
    //Yamami テストロジック

    int reti;

    char buf[1024];
    memset(buf, 0, sizeof(buf));

    sprintf(buf , "protType:%04x \n",1234);

    //FileOutputStream のテスト
    FileOutputStream fos("YTEST.TXT");

    printf("fileout:open=%d\n", fos.open());

    reti = fos.write((byte*)buf , 512);
    printf("fileout:read=%d\n", reti);

    printf("fos close pre\n");
    fos.close();
    printf("fos close after\n");
    return 0;
}

#endif

#ifdef MAIN_2

//Yamami Pciライブラリ使用テスト
int MonaMain(List<char*>* pekoe)
{
    PciInf *pciinfo;
    
    //PCIライブラリクラスのインスタンス化
    Pci* pcilib = new Pci();
    
    pciinfo = pcilib->CheckPciExist(0x10EC,0x8029);
    
    if(pciinfo->Exist == 0){
        //デバイスが見つかれば、その情報を表示
        //printf("VendorName=%s\n",(const char*)pciinfo->VendorName);
        //printf("DeviceName=%s\n",(const char*)pciinfo->DeviceName);
        printf("DeviceNo=%d\n",pciinfo->DeviceNo);
        printf("BaseAd=%x\n",pciinfo->BaseAd);
        printf("IrqLine=%x\n",pciinfo->IrqLine);
    }
    else{
        printf("Device Not Exist!! \n");
    }
    
    
    pciinfo = pcilib->CheckPciExist(0x10AA,0x80AA);
    
    if(pciinfo->Exist == 0){
        //デバイスが見つかれば、その情報を表示
        //printf("VendorName=%s\n",(const char*)pciinfo->VendorName);
        //printf("DeviceName=%s\n",(const char*)pciinfo->DeviceName);
        printf("DeviceNo=%d\n",pciinfo->DeviceNo);
        printf("BaseAd=%x\n",pciinfo->BaseAd);
        printf("IrqLine=%x\n",pciinfo->IrqLine);
    }
    else{
        printf("Device Not Exist!! \n");
    }
    
    return 0;

}

#endif


#ifdef MAIN_3

struct REPLY_WAIT{
    int ip;         /* Request IP address. */
    int repFlg;         /* Reply flag. */
    int wait;
    char mac[6];        /* Reply mac buffer. */
};

//HLIST使用テスト
int MonaMain(List<char*>* pekoe)
{
    List<REPLY_WAIT*>* testList;
    REPLY_WAIT* addWork;
    
    testList = new HList<REPLY_WAIT*>();
    
    
    for (int i = 0; i < 1000; i++) {
        addWork = new REPLY_WAIT();
        addWork->ip = i;
        addWork->wait = i;
        sprintf(addWork->mac , "%d",i*2);
        
        //ここで、HListへ追加
        testList->add(addWork);
    }
    
//    for (int i = 0; i < testList->size() ; i++) {
//
//        REPLY_WAIT* p = testList->get(i);
//        
//        printf("ip=%d wait=%d mac=%s \n", p->ip , p->wait , p->mac);
//        
//        //この後、ここでリストから削除するとどうなる？
//        testList->removeAt(i);
//        //カウンタデクリメントがいる。
//        i--;
//        
//    }
    
    REPLY_WAIT* getWork;
    
    //2個だけとりだしてみる。
    for (int i = 0; i < 2 ; i++) {
        getWork = testList->removeAt(0);
        printf("ip=%d wait=%d mac=%s \n", getWork->ip , getWork->wait , getWork->mac);
    }
    
    //途中追加
    addWork = new REPLY_WAIT();
    addWork->ip = 5;
    addWork->wait = 5;
    sprintf(addWork->mac , "%d",5*2);
    
    //ここで、HListへ追加
    testList->add(addWork);
    
    
    //先頭要素を、とり続ける限り取り出す。
    while(testList->isEmpty() == false){
        getWork = testList->removeAt(0);
        logprintf("ip=%d wait=%d mac=%s \n", getWork->ip , getWork->wait , getWork->mac);
        logprintf("count=%d\n",testList->size());
    }
    
    
    printf("testList->size() = %d\n",testList->size());
    
    return 0;

}


#endif


#ifdef MAIN_4


//Yamami 自己メッセージ通知テスト
int MonaMain(List<char*>* pekoe)
{

    MessageInfo info;

    // テストサーバのIDを見つける
    dword targetID = Message::lookupMainThread("YAMAS.EX2");
    if (targetID == 0xFFFFFFFF)
    {
        printf("local!!!! yamas:INIT not found\n");
        exit(1);
    }

    // create message
    Message::create(&info, MSG_YAMATEST, 0, 0, 0, NULL);
    // send
    if (Message::send(targetID, &info)) {
        printf("local!!!! yamas:INIT error\n");
    }

    
    return 0;

}

#endif



#ifdef MAIN_5


struct REPLY_WAIT{
    int ip;         /* Request IP address. */
    int repFlg;         /* Reply flag. */
    int wait;
    char mac[6];        /* Reply mac buffer. */
};

//HashMap使用テスト
int MonaMain(List<char*>* pekoe)
{
    HashMap<REPLY_WAIT*>* testHash;
    
    REPLY_WAIT* addWork;
    char IpKey[10];    //

    
    testHash = new HashMap<REPLY_WAIT*>(3);
    
    //for (int i = 0; i < 3; i++) {
    for (int i = 0; i < 5; i++) {
        //要素数を超えるとどうなる？？
        addWork = new REPLY_WAIT();
        addWork->ip = i;
        addWork->wait = i;
        sprintf(addWork->mac , "%d",i*2);
        
        //IPアドレスを、MAPのキー化(String化)
        sprintf(IpKey , "%08x",i);
        
        //ここで、HashMapへ追加
        testHash->put(IpKey , addWork);
        
    }
    
    //for (int i = 0; i < testHash->size() ; i++) {
    for (int i = 0; i < 5 ; i++) {

        //IPアドレスを、MAPのキー化(String化)
        sprintf(IpKey , "%08x",i);

        REPLY_WAIT* p = testHash->get(IpKey);
        
        printf("ip=%d wait=%d mac=%s \n", p->ip , p->wait , p->mac);
        
    }
    
    return 0;

}

#endif



#ifdef MAIN_6

struct REPLY_WAIT{
    int ip;         /* Request IP address. */
    int repFlg;         /* Reply flag. */
    int wait;
    char mac[6];        /* Reply mac buffer. */
};

//共有メモリテスト
int MonaMain(List<char*>* pekoe)
{
    
    //まず、monapi_cmemoryinfo構造体をnew
    monapi_cmemoryinfo* cmInfo = new monapi_cmemoryinfo();
    
    REPLY_WAIT* addWork;
    
    if (!monapi_cmemoryinfo_create(cmInfo, (dword)(sizeof(REPLY_WAIT) + 1), 0))
    {
        monapi_cmemoryinfo_delete(cmInfo);
        printf("monapi_cmemoryinfo_create error\n");
    }
    
    //この時点で何がセットされている？
    logprintf("monapi_cmemoryinfo_create after!!!\n");
    
    logprintf("cmInfo->Handle = %x\n",cmInfo->Handle);
    logprintf("cmInfo->Owner = %x\n",cmInfo->Owner);
    logprintf("cmInfo->Size = %x\n",cmInfo->Size);
    //共有メモリをマップ、Data要素に確保したバイト列がセットされる。
    monapi_cmemoryinfo_map(cmInfo);
    
    logprintf("monapi_cmemoryinfo_map after!!!\n");
    
    //実際に共有メモリへ、何かをセット
    //addWork = new REPLY_WAIT();
    //共有メモリ構造体のDataを、目的の型へキャスト
    addWork = (REPLY_WAIT*)cmInfo->Data;
    
    addWork->ip = 10;
    addWork->wait = 1;
    sprintf(addWork->mac , "%d",123);
    
    logprintf("cmemory use!! after!!!\n");
    
    logprintf("ClintSide!! ip=%d wait=%d mac=%s \n", addWork->ip , addWork->wait , addWork->mac);
    

    //ここで、テスト用サーバに共有メモリのHandleを送る
    MessageInfo info;

    dword targetID = Message::lookupMainThread("YAMAS.EX2");
    if (targetID == 0xFFFFFFFF)
    {
        printf("local!!!! yamas:INIT not found\n");
        exit(1);
    }

    // create message
    Message::create(&info, MSG_YAMATEST, cmInfo->Handle, cmInfo->Size, 0, NULL);
    // send
    if (Message::send(targetID, &info)) {
        printf("local!!!! yamas:INIT error\n");
    }

    
    
    return 0;

}

#endif




#ifdef MAIN_7

//IP関連のヘッダはどこに定義すべきか？
//とりあえず、Mones内のMoDefineを強引にinclude
#include "../../../src/servers/mones/MonesDefine.h"


//Code:Mones IP送信テスト
int MonaMain(List<char*>* pekoe)
{

    if(pekoe->size() < 1){
        printf("Arguments is few!\n");
        printf("usage: ping host\n");
        return 0;
    }
    
    
    //printf("pekoe->get(0)=%s\n",pekoe->get(0));
    
    int a,b,c,d;
    
    sscanf(pekoe->get(0),"%d.%d.%d.%d",&a,&b,&c,&d);
    
    //printf("a=%d\n",a);
    //printf("b=%d\n",b);
    //printf("c=%d\n",c);
    //printf("d=%d\n",d);

    //IPアドレス組み立て
    dword ipaddr;
    ipaddr = 0;
    ipaddr = (byte)a;
    ipaddr = (ipaddr << 8) + (byte)b;
    ipaddr = (ipaddr << 8) + (byte)c;
    ipaddr = (ipaddr << 8) + (byte)d;
    
    printf("Pinding %s\n",pekoe->get(0));

    //ここで、Monesにメッセージを送る
    MessageInfo info;

    dword targetID = Message::lookupMainThread("MONES.EX5");
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
    //icmpHead->chksum=MoPacUtl::calcCheckSum((dword*)icmpHead,icmp_size);

    info.arg1 = 0;  //共有メモリを使用しない
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
                printf("Reply from %s\n",pekoe->get(0));
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

#endif


#ifdef MAIN_8

//logprintf の'%' 出力テスト
int MonaMain(List<char*>* pekoe)
{

    printf("%c",'%');  //正常
    printf("%c",'?');  //正常
    printf("%c",'\\'); //正常
    printf("%");       //これは出ないけど、それで正常。
    printf("\n");

    logprintf("%c",'%');  //×出力されない!!!
    logprintf("%c",'?');  //正常
    logprintf("%c",'\\'); //正常
    logprintf("%");       //これは出ないけど、それで正常。
    logprintf("\n");

    char pacbuf[128];
    char drawBuff[128];
    
    pacbuf[0] = 0x7d;
    pacbuf[1] = 0x8d;
    
    
    for(int i = 0 ; i < 2 ; i++){
        sprintf(drawBuff , "%2x",(byte)pacbuf[i]);
        printf("%s\n",drawBuff);
    }
    
    return 0;
}

#endif
