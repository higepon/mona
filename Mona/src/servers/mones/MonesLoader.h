/*!
    \file  MonesLoader.h
    \brief  Mones NICドライバインスタンス ローダー

    Copyright (c) 2004 Yamami
    All rights reserved.
    License=MIT/X Licnese

    \author  Yamami 
    \version $Revision$
    \date   create:2004/10/31 update:$Date$
*/
#ifndef _MONA_MONESLOADER_
#define _MONA_MONESLOADER_

#include <sys/types.h>
#include <sys/HashMap.h>
#include <monapi.h>

#include "MoPacUtl.h"
#include "MonesDefine.h"
#include "AbstractMonic.h"


/*!
    MonesLoader class
*/
class MonesLoader
{
    
  public:
    
    //コンストラクタ
    MonesLoader();
    ~MonesLoader();
    
    void setup();
    AbstractMonic* getNicInstance();

  private:

};

#endif
