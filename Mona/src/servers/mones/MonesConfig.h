/*!
    \file  MonesConfig.h
    \brief ネットワーク設定

    Copyright (c) 2004 Yamami
    All rights reserved.
    License=MIT/X Licnese

    \author  Yamami 
    \version $Revision$
    \date   create:2004/08/28 update:$Date$
*/

#ifndef _MONA_MONESCONFIG_
#define _MONA_MONESCONFIG_

#include <sys/types.h>

/*!
    class MonesConfig
*/
class MonesConfig {


  private:
  
    //ネットワークコンフィグ 当面ここでコンスタント設定
    //IPアドレス 
    //*****************************************
    //MonaのIPアドレス設定
    //IPアドレス long 2バイトずつ合計4バイト
    // 例:192(C0).168(A8).9(01).1(01) → 0xC0A80101
    //*****************************************
    static const dword   gl_myIpAdr = 0xC0A80101;       //IPアドレス;
    
    //サブネットマスク 
    static const dword   gl_mySubnet = 0xFFFFFF00;

    //GW
    static const dword   gl_myGw = 0xC0A801FE;

    //DNS
    static const dword   gl_myDns = 0xC0A801FE;
    
    //MTU  Yamami これは、NICごとに持つべき？？
    static const int   gl_myMTU = 1500;
    
  public:
    MonesConfig();
    ~MonesConfig();
  
    //IPアドレスゲッター
    dword getGl_myIpAdr();
    
    //サブネットマスクゲッター
    dword getGl_mySubnet();
    //GWゲッター
    dword getGl_myGw();
    //DNSアドレスゲッター
    dword getGl_myDns();

    //MTU値 ゲッター
    dword getGl_myMTU();

};

#endif
