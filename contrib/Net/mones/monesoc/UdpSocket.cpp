/*!
    \file   UdpSocket.cpp
    \brief  UdpSocketクラス

    Copyright (c) 2005 Yamami
    All rights reserved.
    License=MIT/X License

    \author  Yamami
    \version $Revision$
    \date   create:2005/08/09 update:$Date$
*/

/*! \class UdpSocket
 *  \brief UdpSocketクラス
 */


#include <monesoc/UdpSocket.h>

using namespace MonAPI;

/*!
    \brief UdpSocket
         UdpSocket コンストラクタ
    \author Yamami
    \date   create:2005/08/09 update:$Date$
*/
UdpSocket::UdpSocket()
{
    useport=0;
}

/*!
    \brief ~UdpSocket
         UdpSocket デスクトラクタ
    \author Yamami
    \date   create:2005/08/09 update:$Date$
*/
UdpSocket::~UdpSocket()
{
 
}


/*!
    \brief bind
         Socketを指定ポートにbind
    \param  uint16_t port [in] ポート番号
    
    \return int 結果 
    
    \author Yamami
    \date   create:2005/08/09 update:$Date$
*/
int UdpSocket::bind(uint16_t port)
{
    //ここで、Monesに登録メッセージを送る
    MessageInfo info;

    uint32_t targetID = Message::lookupMainThread("MONES.EX5");
    if (targetID == 0xFFFFFFFF)
    {
        logprintf("MONES.EX5 not found\n");
        return 1;
    }

    //Monesへ登録
    //ポート2600でリスン
    Message::create(&info, MSG_MONES_REGIST, 0, 2600, 0, NULL);
    // send
    if (Message::send(targetID, &info)) {
        logprintf("MSG_MONES_REGIST error\n");
        return 1;
    }
    
    //useポートは、bindしたポートで兼用する。
    useport = port;
    
    return 0;
}


/*!
    \brief recv
         bind済みポートからの受信
         バッファサイズより大きいデータは破棄されます。
    \param  char* buff [OUT] 受信バッファ
    \param  uint32_t len [in] バッファサイズ    
    \return int 結果 
    
    \author Yamami
    \date   create:2005/08/09 update:$Date$
*/
int UdpSocket::recv(char* buff ,uint32_t len)
{

    MessageInfo info;

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
                
                SocketContainer *soc;
                
                //共有メモリ
                monapi_cmemoryinfo* cmSoc;

                cmSoc = monapi_cmemoryinfo_new();
                cmSoc->Handle = info.arg2;
                cmSoc->Size   = info.arg3;
                
                monapi_cmemoryinfo_map(cmSoc);
                
                //共有メモリからSocketContainerを取り出す。
                soc = (SocketContainer*)cmSoc->Data;

                //SocketContainerから、パケット(バイト列)を取り出す。
                //UDPは、最大超えると読み捨てる。TCPはフロー制御が必要 UDPSocket とTCPSocketの仕事
                packet_cmemoryinfo pcm;
                
                pcm = soc->getBuffer(0);

                //共有メモリからパケット バイト を取得                
                monapi_cmemoryinfo* cmPacByte;
                
                cmPacByte = monapi_cmemoryinfo_new();
                cmPacByte->Handle = pcm.Handle;
                cmPacByte->Size   = pcm.Size;
                monapi_cmemoryinfo_map(cmPacByte);                
                
                //アプリから渡されているバッファへコピー
                memcpy(buff,cmPacByte->Data , len);                
                
                //SocketContainerは、アプリで共有メモリ解放しない!!
                //monapi_cmemoryinfo_dispose(cmSoc);
                //monapi_cmemoryinfo_delete(cmSoc);                

                //共有メモリ解放
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
}



/*!
    \brief send
         UDP送信
    \param  char* mesg [IN] 送信メッセージバッファ
    \param  uint32_t len [in] バッファサイズ
    \param  char* host [in] 送信ホスト (現在 X.X.X.X 形式のみサポート)
    \param  uint16_t port [in] 送信ポート番号
    \return int 結果 
    
    \author Yamami
    \date   create:2005/08/09 update:$Date$
*/
int UdpSocket::send(char* mesg, uint32_t len, char* host, uint16_t port)
{
    
    MessageInfo info;
    SocketContainer *soc;
    uint32_t ipaddr;

    if(useport == 0){
        //自己ポートがまだ未割り当てなら
        //TODO 本来ならmonesプロトコルサーバに問い合わせ空きポートを取得する!
        // とりあえず、適当に2800番
        useport = 2800;
    }
    
    //IPアドレス組み立て
     int a,b,c,d;
    
    sscanf(host,"%d.%d.%d.%d",&a,&b,&c,&d);
    ipaddr = 0;
    ipaddr = (uint8_t)a;
    ipaddr = (ipaddr << 8) + (uint8_t)b;
    ipaddr = (ipaddr << 8) + (uint8_t)c;
    ipaddr = (ipaddr << 8) + (uint8_t)d;
    
logprintf("UdpSocket::send ipaddr = %x\n",ipaddr);
    
    soc = new SocketContainer();
    soc->tid = MonAPI::System::getThreadID();
    soc->myport = useport;
    soc->ip = ipaddr;
    soc->youport = port; 
    soc->status = 0;  //TODO ここはどう扱う？

    int reti;
    
    //ソケットの送信バッファへ追加
    reti = soc->addBuffer(mesg , len , 1);
    
    //MSG_MONES_IP_SEND メッセージ作成
    Message::create(&info, MSG_MONES_IP_SEND, 0, 0, 0, NULL);
    
    //メッセージにSocketContainerをセット
    reti = soc->createPacMonaMsg(&info , 17);
    
    uint32_t targetID = Message::lookupMainThread("MONES.EX5");
    if (targetID == 0xFFFFFFFF)
    {
        logprintf("MONES.EX5 not found\n");
        return 1;
    }
    
    // send
    if (Message::send(targetID, &info)) {

    }
    
}

