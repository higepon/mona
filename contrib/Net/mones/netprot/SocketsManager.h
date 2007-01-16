/*!
    \file  SocketsManager.h
    \brief SocketContainer群 管理クラス

    Copyright (c) 2004 Yamami
    All rights reserved.
    License=MIT/X License

    \author  Yamami 
    \version $Revision$
    \date   create:2005/07/15 update:$Date$
*/
#ifndef _MONA_SOCKETSMANAGER_
#define _MONA_SOCKETSMANAGER_

#include <sys/types.h>
#include <sys/HashMap.h>
#include <monapi.h>
#include <monalibc/stdio.h>

#include <monesoc/SocketContainer.h>

/* TCP 状態遷移 */
#define     CLOSED      0
#define     LISTEN      0
#define     SYN_RCVD    1
#define     SYN_SENT    1
#define     ESTABLISHED 2
#define     FIN_WAIT_1  4
#define     CLOSE_WAIT  4
#define     LAST_ACK    4
#define     FIN_WAIT_2  5
//#define       TIME_WAIT       6


#define MAX_SOC_SIZE    1024    //最大コネクション数  

/*!
    SocketsManager class
*/
class SocketsManager
{
    
  public:
    
    //コンストラクタ
    SocketsManager();
    ~SocketsManager();
    
    SocketContainer* findSocket(uint32_t ip ,uint16_t  myport ,uint16_t youport);
    void registLisSocket(uint32_t tid ,uint16_t  myport );
    void registConSocket(uint32_t tid ,uint16_t myport, uint32_t ip , uint16_t youport);

  private:
    //コネクション中のSocketContainerハッシュ
    HashMap<SocketContainer*> *conSocHash;
    
    //リスン中のSocketContainerハッシュ
    HashMap<SocketContainer*> *lisSocHash;

};

#endif
