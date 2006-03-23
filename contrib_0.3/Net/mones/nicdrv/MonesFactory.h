/*!
    \file  MonesFactory.h
    \brief  Mones NICドライバインスタンス ローダー

    Copyright (c) 2004 Yamami
    All rights reserved.
    License=MIT/X License

    \author  Yamami 
    \version $Revision$
    \date   create:2004/10/31 update:$Date$
*/
#ifndef _MONA_MonesFactory_
#define _MONA_MonesFactory_

#include <sys/types.h>
#include <sys/HashMap.h>
#include <monapi.h>

//#include "MoPacUtl.h"
//#include "MonesDefine.h"
#include <nicdrv/AbstractMonic.h>


/*!
    MonesFactory class
*/
class MonesFactory
{
    
  public:
    
    //コンストラクタ
    MonesFactory();
    ~MonesFactory();
    
    void setup();
    AbstractMonic* getNicInstance();

  private:

};

#endif
