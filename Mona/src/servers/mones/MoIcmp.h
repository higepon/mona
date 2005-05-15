/*!
    \file  MoIcmp.h
    \brief ICMPプロトコル実装クラス ヘッダ

    Copyright (c) 2004 Yamami
    All rights reserved.
    License=MIT/X License

    \author  Yamami 
    \version $Revision$
    \date   create:2004/09/20 update:$Date$
*/
#ifndef _MONA_MOICMP_
#define _MONA_MOICMP_

#include <sys/types.h>
#include <sys/HashMap.h>
#include <monapi.h>

#include "MoPacUtl.h"
#include "MonesDefine.h"
#include "AbstractMonic.h"
#include "MoIp.h"


/*!
    MoIcmp class
*/
class MoIcmp
{
    
  public:
    
    //コンストラクタ
    MoIcmp();
    ~MoIcmp();
    void initIp(AbstractMonic*);

    int receiveIcmp(IP_HEADER*);

  private:
    
    void transIcmp(dword , byte , byte , ICMP_HEADER*, int );
    void saveRecv(IP_HEADER*, int );
    //NICドライバ
    AbstractMonic* insAbstractNic;

};

#endif
