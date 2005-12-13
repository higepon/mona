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


//#include "AbstractMonic.h"
#include <AbstractMonic.h>

using namespace MonAPI;

/*!
    \brief AbstractMonic コンストラクタ

    \author Yamami
    \date   create:2004/09/04 update:
*/
AbstractMonic::AbstractMonic()
{
    waitFrameBufList = new HList<WAIT_FRAME_BUF*>();
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
        outp8(0x21, (inp8(0x21) | IrqMask));
    }
    else if(8 <= nicIrqNo && nicIrqNo <= 15){
    
        //IRQの範囲が、8～15であれば
        //PIC-1スレーブのIMR(0xA1)へ割り込みマスク
        IrqMask = 1 << (nicIrqNo - 8);
        outp8(0xA1, (inp8(0xA1) | IrqMask));
    
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
        outp8(0x21, (inp8(0x21) & IrqMask));
    }
    else if(8 <= nicIrqNo && nicIrqNo <= 15){
    
        //IRQのカスケードをマスク
        outp8(0x21, (inp8(0x21) & 0xFB)); /* IR2 cascade */
    
        //IRQの範囲が、8～15であれば
        //PIC-1スレーブのIMR(0xA1)へ割り込みマスク
        IrqMask = IrqMask ^ (1 << (nicIrqNo - 8));  //XOR 当該ビットのみ0とする。
        outp8(0xA1, (inp8(0xA1) & IrqMask));
    }
    
}

/*!
    \brief フレーム受信処理 

    \author Yamami
    \date   create:2005/05/22 update:
*/
void AbstractMonic::frame_input_public() 
{

    //フレーム受信処理 H/W 子クラスへ委譲
    //frame_buf[1500] へ格納される事を期待
    frame_input();
    
    
    //処理待ちリストへ登録
    //サイズ分の共有メモリを確保し、保存。ハンドルをリストへ登録
    //まず、monapi_cmemoryinfo構造体をnew
    monapi_cmemoryinfo* cmInfo = new monapi_cmemoryinfo();
    
    if (!monapi_cmemoryinfo_create(cmInfo, frame_len , 0)){
        monapi_cmemoryinfo_delete(cmInfo);
        //TODO 確保できなかったらどうする？
        return;
    }
    
    //共有メモリをマップ、Data要素に確保したバイト列がセットされる。
    monapi_cmemoryinfo_map(cmInfo);
    
    //共有メモリへ、パケットセット
    memcpy(cmInfo->Data , frame_buf, frame_len);
    
    //リストへ登録
    WAIT_FRAME_BUF *addWork = new WAIT_FRAME_BUF();
    addWork->cmHandle = cmInfo->Handle;
    addWork->cmSize = cmInfo->Size;
    
    waitFrameBufList->add(addWork);
    
    //プロトコル層へ処理要求メッセージ送信
    dword targetID = Message::lookupMainThread("MONES.EX5");
    if (targetID == 0xFFFFFFFF){
        //TODO できなかったらどうする？
    }

    MessageInfo info;
    Message::create(&info, MSG_MONES_FRAME_REQ, 0, 0, 0, NULL);

    // send
    if (Message::send(targetID, &info)) {
        //TODO できなかったらどうする？
    }
    

}
