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
    \return uint32_t IPアドレス
    \author Yamami
    \date   create:2004/08/29 update:$Date$
*/
uint32_t MonesConfig::getGl_myIpAdr( )
{
    
    uint32_t retIp;
    
    //メンバの内容をそのままリターン
    retIp = gl_myIpAdr;
    
    return retIp;
}


/*!
    \brief getGl_myIpAdr
        サブネットマスク

    \param  無し
    \return uint32_t サブネットマスク
    \author Yamami
    \date   create:2004/09/20 update:$Date$
*/
uint32_t MonesConfig::getGl_mySubnet( )
{
    
    uint32_t retSubnet;
    
    //メンバの内容をそのままリターン
    retSubnet = gl_mySubnet;
    
    return retSubnet;
}


/*!
    \brief getGl_myGw
        GW

    \param  無し
    \return uint32_t GW
    \author Yamami
    \date   create:2004/09/20 update:$Date$
*/
uint32_t MonesConfig::getGl_myGw( )
{
    
    uint32_t retGw;
    
    //メンバの内容をそのままリターン
    retGw = gl_myGw;
    
    return retGw;
}


/*!
    \brief getGl_myDns
        DNS

    \param  無し
    \return uint32_t DNS
    \author Yamami
    \date   create:2004/09/20 update:$Date$
*/
uint32_t MonesConfig::getGl_myDns( )
{
    
    uint32_t retDNS;
    
    //メンバの内容をそのままリターン
    retDNS = gl_myDns;
    
    return retDNS;
}


/*!
    \brief getGl_myMTU
        MTU

    \param  無し
    \return uint32_t MTU
    \author Yamami
    \date   create:2004/09/20 update:$Date$
*/
uint32_t MonesConfig::getGl_myMTU( )
{
    
    uint32_t retMTU;
    
    //メンバの内容をそのままリターン
    retMTU = gl_myMTU;
    
    return retMTU;
}
