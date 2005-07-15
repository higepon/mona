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
#ifndef _MONA_SOCKET_
#define _MONA_SOCKET_

#include <sys/types.h>
#include <sys/HashMap.h>
#include <monapi.h>

#include "MoPacUtl.h"
#include "MonesDefine.h"
#include "AbstractMonic.h"




/*!
    Socket class
*/
class Socket
{
    
  public:
    
    //コンストラクタ
    Socket();
    ~Socket();
    
    byte   status;
    dword       ip;
    word    myport;  //自己ポート
    word    youport; //相手ポート
    dword   seq;    // 自分からのシーケンス番号
    dword   ack;    // 相手からのシーケンス番号


  private:


};

#endif
