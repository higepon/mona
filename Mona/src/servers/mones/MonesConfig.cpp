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


/*!
    \brief getGl_myIpAdr
        サブネットマスク

    \param  無し
    \return dword サブネットマスク
    \author Yamami
    \date   create:2004/09/20 update:$Date$
*/
dword MonesConfig::getGl_mySubnet( )
{
    
    dword retSubnet;
    
    //メンバの内容をそのままリターン
    retSubnet = gl_mySubnet;
    
    return retSubnet;
}


/*!
    \brief getGl_myGw
        GW

    \param  無し
    \return dword GW
    \author Yamami
    \date   create:2004/09/20 update:$Date$
*/
dword MonesConfig::getGl_myGw( )
{
    
    dword retGw;
    
    //メンバの内容をそのままリターン
    retGw = gl_myGw;
    
    return retGw;
}


/*!
    \brief getGl_myDns
        DNS

    \param  無し
    \return dword DNS
    \author Yamami
    \date   create:2004/09/20 update:$Date$
*/
dword MonesConfig::getGl_myDns( )
{
    
    dword retDNS;
    
    //メンバの内容をそのままリターン
    retDNS = gl_myDns;
    
    return retDNS;
}


/*!
    \brief getGl_myMTU
        MTU

    \param  無し
    \return dword MTU
    \author Yamami
    \date   create:2004/09/20 update:$Date$
*/
dword MonesConfig::getGl_myMTU( )
{
    
    dword retMTU;
    
    //メンバの内容をそのままリターン
    retMTU = gl_myMTU;
    
    return retMTU;
}
