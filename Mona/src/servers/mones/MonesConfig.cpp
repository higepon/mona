/*!
    \file  MonesConfig.cpp
    \brief ネットワーク設定

    Copyright (c) 2004 Yamami
    WITHOUT ANY WARRANTY

    \author  Yamami
    \version $Revision$
    \date   create:2004/08/29 update:$Date$
*/

/*! \class MonesConfig
 *  \brief ネットワーク設定
 */



#include "MonesConfig.h"


/*!
    \brief MonesConfig コンストラクタ

    \author Yamami
    \date   create:2004/08/29 update:$Date$
*/
MonesConfig::MonesConfig()
{
    

}


/*!
    \brief MonesConfig デスクトラクタ

    \author Yamami
    \date   create:2004/08/29 update:$Date$
*/
MonesConfig::~MonesConfig()
{
    
}


/*!
    \brief getGl_myIpAdr
        IPアドレスゲッター

    \param  無し
    \return dword IPアドレス
    \author Yamami
    \date   create:2004/08/29 update:$Date$
*/
dword MonesConfig::getGl_myIpAdr( )
{
    
    dword retIp;
    
    //メンバの内容をそのままリターン
    retIp = gl_myIpAdr;
    
    return retIp;
}

