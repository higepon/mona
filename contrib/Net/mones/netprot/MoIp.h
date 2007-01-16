/*!
    \file  MoIp.h
    \brief IPプロトコル実装クラス ヘッダ

    Copyright (c) 2004 Yamami
    All rights reserved.
    License=MIT/X License

    \author  Yamami 
    \version $Revision$
    \date   create:2004/09/20 update:$Date$
*/
#ifndef _MONA_MOIP_
#define _MONA_MOIP_

#include <sys/types.h>
#include <sys/HashMap.h>
#include <monapi.h>

#include "MoPacUtl.h"
#include "MonesDefine.h"
#include <nicdrv/AbstractMonic.h>


/*!
    MoIp class
*/
class MoIp
{
    
  public:
    
    //コンストラクタ
    MoIp();
    ~MoIp();
    void initIp(AbstractMonic*);

    int receiveIp(IP_HEADER*);
    int transIp(TRANS_BUF_INFO*, uint32_t , uint8_t , int );

  private:
    int ipRouting(uint32_t ,uint32_t*);
    
    //NICドライバ
    AbstractMonic* insAbstractNic;

};

#endif
