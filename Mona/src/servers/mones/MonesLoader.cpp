/*!
    \file   MonesLoader.cpp
    \brief  Mones NICドライバインスタンス ローダー

    Copyright (c) 2004 Yamami
    All rights reserved.
    License=MIT/X License

    \author  Yamami
    \version $Revision$
    \date   create:2004/10/31 update:$Date$
*/

/*! \class MonesLoader
 *  \brief Mones NICドライバインスタンス ローダー
 */


#include <pci/Pci.h>

#include "MonesLoader.h"
#include "MonesConfig.h"
#include "MonesGlobal.h"
#include "AbstractMonic.h"
#include "Ne2000MoNic.h"

/*!
    \brief initialize
         MonesLoader コンストラクタ
    \author Yamami
    \date   create:2004/10/31 update:$Date$
*/
MonesLoader::MonesLoader()
{

}


/*!
    \brief initialize
         MonesLoader デスクトラクタ
    \author Yamami
    \date   create:2004/10/31 update:$Date$
*/
MonesLoader::~MonesLoader() 
{

}


/*!
    \brief setup
         MonesLoader setup
    \author Yamami
    \date   create:2004/10/31 update:$Date$
*/
void MonesLoader::setup() 
{
    //何もしない。

}


/*!
    \brief getNicInstance
         NICドライバのインスタンスを得る。
    \author Yamami
    \date   create:2004/10/31 update:$Date$
*/
AbstractMonic* MonesLoader::getNicInstance() 
{
    
    //TODO 2004/10/31 現在
    // PCIデバイス検索して、見つかれば QEMU / 見つからなければ Bochs と判断し
    // NE2000ドライバをロードする。
    // その他環境では、ロードに失敗
    
    int ret;
    PciInf *pciinfo;              //PCIライブラリ

    Ne2000MoNic *nicDriver;
    
    //PCIライブラリクラスのインスタンス化
    Pci* pcilib = new Pci();
    
    //TODO 一時しのぎなので、マジックナンバーを用いる
    //QEMU上の、NE2000を探す
    pciinfo = pcilib->CheckPciExist(0x10EC,0x8029);
    
    if(pciinfo->Exist == 0){
        //見つかれば
        //NE2000のロード
        nicDriver = new Ne2000MoNic();
        
        //QEMU設定
        nicDriver->setNicIRQ(9);
        nicDriver->setNicIOBASE(0xC100);
        
    }
    else{
        //見つからなければ
        //NE2000のロード
        nicDriver = new Ne2000MoNic();
        
        //Bochs設定
        nicDriver->setNicIRQ(3);
        nicDriver->setNicIOBASE(0x240);
    
    }
    
    
    //NIC初期化
    ret = nicDriver->init();
    
    //if(ret != 0){
    //    //初期化に失敗した場合、NULLドライバを返す。
    //    nicDriver = 0;
    //    
    //}
    
    
    return nicDriver;
}

