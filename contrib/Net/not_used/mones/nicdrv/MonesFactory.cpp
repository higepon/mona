/*!
    \file   MonesFactory.cpp
    \brief  Mones NICドライバインスタンス ローダー

    Copyright (c) 2004 Yamami
    All rights reserved.
    License=MIT/X License

    \author  Yamami
    \version $Revision$
    \date   create:2004/10/31 update:$Date$
*/

/*! \class MonesFactory
 *  \brief Mones NICドライバインスタンス ローダー
 */


#include <pci/Pci.h>

#include "MonesFactory.h"
#include <nicdrv/AbstractMonic.h>
#include "ne2000/Ne2000MoNic.h"
#include "tulip/tulipMoNic.h"

/*!
    \brief initialize
         MonesFactory コンストラクタ
    \author Yamami
    \date   create:2004/10/31 update:$Date$
*/
MonesFactory::MonesFactory()
{

}


/*!
    \brief initialize
         MonesFactory デスクトラクタ
    \author Yamami
    \date   create:2004/10/31 update:$Date$
*/
MonesFactory::~MonesFactory() 
{

}


/*!
    \brief setup
         MonesFactory setup
    \author Yamami
    \date   create:2004/10/31 update:$Date$
*/
void MonesFactory::setup() 
{
    //何もしない。

}


/*!
    \brief getNicInstance
         NICドライバのインスタンスを得る。
    \author Yamami
    \date   create:2004/10/31 update:$Date$
*/
AbstractMonic* MonesFactory::getNicInstance() 
{
    
    //TODO 2004/10/31 現在
    // PCIデバイス検索して、見つかれば QEMU / 見つからなければ Bochs と判断し
    // NE2000ドライバをロードする。
    // その他環境では、ロードに失敗
    
    int ret;
    //PciInf *pciinfo;              //PCIライブラリ

    PciInf RetPciInf; //返却用 PciInf構造体
    
    AbstractMonic *nicDriver;
    
    //PCIライブラリクラスのインスタンス化
    Pci* pcilib = new Pci();
    
    //TODO 一時しのぎなので、マジックナンバーを用いる
    //QEMU上の、NE2000を探す
    pcilib->CheckPciExist(0x10EC,0x8029,&RetPciInf);
    
    if(RetPciInf.Exist == 0){
        //見つかれば
        //NE2000のロード
        nicDriver = new Ne2000MoNic();
        
        //QEMU設定
        //nicDriver->setNicIRQ(9);
        //nicDriver->setNicIOBASE(0xC100);
        nicDriver->setNicIRQ(RetPciInf.IrqLine);
        nicDriver->setNicIOBASE(RetPciInf.BaseAd -1);
        
    }
    
    //VirtualPC上の、DEC21140を探す
    pcilib->CheckPciExist(0x1011,0x0009,&RetPciInf);
    if(RetPciInf.Exist == 0){
        //見つかれば
        //tulipのロード
        nicDriver = new tulipMoNic();
        
        //VirtualPC設定
        nicDriver->setNicIRQ(11);
        nicDriver->setNicIOBASE(0xEC00);
        
    }
    
//    else{
//        //見つからなければ
//        //ISA NE2000のロード
//        nicDriver = new Ne2000MoNic();
//        
//        //Bochs設定
//        nicDriver->setNicIRQ(3);
//        nicDriver->setNicIOBASE(0x240);
//    
//    }
    
    
    //NIC初期化
    ret = nicDriver->init();
    
    //if(ret != 0){
    //    //初期化に失敗した場合、NULLドライバを返す。
    //    nicDriver = 0;
    //    
    //}
    
    delete pcilib;
    
    return nicDriver;
}

