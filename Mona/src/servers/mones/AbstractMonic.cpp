/*!
    \file  AbstractMonic.cpp
    \brief 抽象NICクラス

    Copyright (c) 2004 Yamami
    WITHOUT ANY WARRANTY

    \author  Yamami
    \version $Revision$
    \date   create:2004/09/04 update:$Date$
*/


/*! \class AbstractMonic
 *  \brief 抽象NICクラス
 */


#include "AbstractMonic.h"

/*!
    \brief AbstractMonic コンストラクタ

    \author Yamami
    \date   create:2004/09/04 update:
*/
AbstractMonic::AbstractMonic()
{
    //実装無し
}


/*!
    \brief AbstractMonic デスクトラクタ

    \author Yamami
    \date   create:2004/09/04 update:
*/
AbstractMonic::~AbstractMonic()
{
    //実装無し
}


/*!
    \brief disable netRecv interrupt

    \author Yamami
    \date   create:2004/08/08 update:
*/
void AbstractMonic::disableNetWork() 
{

    byte IrqMask;
    int  nicIrqNo;
    
    IrqMask = 0x00;
    
    //子クラスのゲッターで、IRQ取得
    nicIrqNo = getNicIRQ();
    
    if(0 <= nicIrqNo && nicIrqNo <= 7){
        //IRQの範囲が、0～7であれば
        //PIC-0マスタのIMR(0x21)へ割り込みマスク
        IrqMask = 1 << (nicIrqNo - 0);
        outp8(0x21, inp8(0x21) | IrqMask);
    }
    else if(8 <= nicIrqNo && nicIrqNo <= 15){
    
        //IRQの範囲が、8～15であれば
        //PIC-1スレーブのIMR(0xA1)へ割り込みマスク
        IrqMask = 1 << (nicIrqNo - 8);
        outp8(0xA1, inp8(0x21) | IrqMask);
    
    }
    
}


/*!
    \brief enable netRecv interrupt

    \author Yamami
    \date   create:2004/08/08 update:
*/
void AbstractMonic::enableNetWork() 
{

    byte IrqMask;
    int  nicIrqNo;
    
    IrqMask = 0xFF;

    //子クラスのゲッターで、IRQ取得
    nicIrqNo = getNicIRQ();

    if(0 <= nicIrqNo && nicIrqNo <= 7){
        //IRQの範囲が、0～7であれば
        //PIC-0マスタのIMR(0x21)へ割り込みマスク
         IrqMask = IrqMask ^ (1 << (nicIrqNo - 0));  //XOR 当該ビットのみ0とする。
printf("IrqMask = %x \n",IrqMask);
        outp8(0x21, inp8(0x21) & IrqMask);
    }
    else if(8 <= nicIrqNo && nicIrqNo <= 15){
    
        //IRQのカスケードをマスク
        outp8(0x21, (inp8(0x21) & 0xFB)); /* IR2 cascade */
    
        //IRQの範囲が、8～15であれば
        //PIC-1スレーブのIMR(0xA1)へ割り込みマスク
        IrqMask = IrqMask ^ (1 << (nicIrqNo - 8));  //XOR 当該ビットのみ0とする。
printf("IrqMask Set = %x \n",IrqMask);
        outp8(0xA1, inp8(0xA1) & IrqMask);
    }
    
    IrqMask=inp8( 0xA1 );
    
//Yamami!!! わざとバグ    
//outp8(0x21, inp8(0x21) | 0x04);

printf("0xA1 IN After = %x \n",IrqMask);
    
}
