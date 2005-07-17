/*!
    \file  SocketsManager.h
    \brief Socket群 管理クラス

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

#include "MoPacUtl.h"
#include "MonesDefine.h"
#include "AbstractMonic.h"
#include "Socket.h"



/*!
    SocketsManager class
*/
class SocketsManager
{
    
  public:
    
    //コンストラクタ
    SocketsManager();
    ~SocketsManager();
    

  private:
    //コネクション中のSocketハッシュ
    HashMap<Socket*>* conSocList;
    
    //リスン中のSocketハッシュ
    HashMap<Socket*>* lisSocList;

};

#endif
