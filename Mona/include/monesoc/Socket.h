/*!
    \file  Socket.h
    \brief 簡易Socketクラス ヘッダ

    Copyright (c) 2004 Yamami
    All rights reserved.
    License=MIT/X License

    \author  Yamami 
    \version $Revision$
    \date   create:2005/07/15 update:$Date$
*/

#include <sys/types.h>
#include <sys/HashMap.h>
#include <monapi.h>
#include <monapi/messages.h>

#ifndef _MONA_SOCKET_
#define _MONA_SOCKET_


#define MAX_BUFF_SIZE    128    //最大バッファ数

typedef struct
{
    dword Handle, Owner, Size;
} packet_cmemoryinfo;

/*!
    Socket class
*/
class Socket
{
    
  public:
    
    //コンストラクタ
    Socket();
    ~Socket();
    
    byte    status;      //遷移状態
    dword   ip;
    word    myport;  //自己ポート
    word    youport; //相手ポート
    dword   seq;    // 自分からのシーケンス番号
    dword   ack;    // 相手からのシーケンス番号
    dword   tid;    //スレッドID
    
    //HList<packet_cmemoryinfo*> *recvBuff;   //受信バッファ ネット→Mones ○→アプリ
    //HList<packet_cmemoryinfo*> *sendBuff;   //送信バッファ アプリ→Mones ○→ネット
    
    //共有メモリでやりとりする入れ物なので、固定確保
    packet_cmemoryinfo recvBuff[MAX_BUFF_SIZE];
    packet_cmemoryinfo sendBuff[MAX_BUFF_SIZE];
    
    int addBuffer(char *pac , int pacsize ,int rorsFlag);
    
    packet_cmemoryinfo getBuffer(int rorsFlag);

  private:
    int nowRecvBufCount;
    int nowSendBufCount;
    
};

#endif
