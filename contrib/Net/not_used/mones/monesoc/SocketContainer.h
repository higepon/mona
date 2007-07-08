/*!
    \file  SocketContainer.h
    \brief SocketContainerクラス ヘッダ

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
#include <monapi/cmemoryinfo.h>

#ifndef _MONA_SOCKETCONTAINER_
#define _MONA_SOCKETCONTAINER_


#define MAX_BUFF_SIZE    128    //最大バッファ数

typedef struct
{
    uint32_t Handle, Owner, Size;
} packet_cmemoryinfo;

/*!
    SocketContainer class
*/
class SocketContainer
{
    
  public:
    
    //コンストラクタ
    SocketContainer();
    ~SocketContainer();
    
    uint8_t    status;      //遷移状態
    uint32_t   ip;
    uint16_t    myport;  //自己ポート
    uint16_t    youport; //相手ポート
    uint32_t   seq;    // 自分からのシーケンス番号
    uint32_t   ack;    // 相手からのシーケンス番号
    uint32_t   tid;    //スレッドID
    
    
    //共有メモリでやりとりする入れ物なので、固定確保
    packet_cmemoryinfo recvBuff[MAX_BUFF_SIZE];
    packet_cmemoryinfo sendBuff[MAX_BUFF_SIZE];
    
    int addBuffer(char *pac , int pacsize ,int rorsFlag);
    
    packet_cmemoryinfo getBuffer(int rorsFlag);
    
    int createPacMonaMsg(MessageInfo *info , uint32_t protno);

  private:
    int nowRecvBufCount;
    int nowSendBufCount;
    
};

#endif
