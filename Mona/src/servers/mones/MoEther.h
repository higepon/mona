/*!
    \file  MoEther.h
    \brief イーサネット処理クラス ヘッダ

    class MoEther

    Copyright (c) 2004 Yamami
    All rights reserved.
    License=MIT/X Licnese

    \author  Yamami 
    \version $Revision$
    \date   create:2004/08/12 update:$Date$
*/
#ifndef _MONA_MOETHER_
#define _MONA_MOETHER_

#include <sys/types.h>
#include <monapi.h>

#include "MoPacUtl.h"
#include "MonesDefine.h"

#include "AbstractMonic.h"
#include "MoArp.h"


/*! 
 *  \struct ETHER_FRAME
 *  \brief イーサネットフレーム構造体
 */
typedef struct{
    byte  dstmac[6];   // 送信先 MAC ID
    byte  srcmac[6];   // 送信元 MAC ID
    word    type;     // フレームタイプ Frame type(DIX) or frame length(IEEE)
    byte   data[0x600];// Data
}ETHER_FRAME;


/*!
    MoEther class
*/
class MoEther
{
    
  public:
    
    //コンストラクタ
    MoEther();
    ~MoEther();
    void etherInit(AbstractMonic*);
    int setEtherFrame(byte* , int);
    int getEtherFrame(ETHER_FRAME* );
    int receiveEther();
    void sendEther(byte* , dword , dword);

  private:
    //イーサネットフレーム保持リスト
    List<ETHER_FRAME*>* Ether_FrameList_;
    
    //NICドライバ
    AbstractMonic* insAbstractNic;
    
};

#endif
