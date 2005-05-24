/*!
    \file  AbstractMonic.h
    \brief 抽象NICクラス ヘッダ
    
    Copyright (c) 2004 Yamami
    WITHOUT ANY WARRANTY

    \author  Yamami
    \version $Revision$
    \date   create:2004/09/04 update:$Date$
*/
#ifndef _MONA_ABSTRACT_MONIC_
#define _MONA_ABSTRACT_MONIC_

#include <sys/types.h>
#include <monapi.h>
#include <monapi/io.h>
//共有メモリ使用
#include <monapi/cmemoryinfo.h>


/*! 
 *  \struct WAIT_FRAME_BUF
 *  \brief 処理待ちフレーム構造体
 */
typedef struct{
    dword cmHandle;  //共有メモリハンドル
    dword cmSize;  //共有メモリサイズ
}WAIT_FRAME_BUF;



/*!
    abstract class AbstractMonic
*/

class AbstractMonic {

  private:

  protected:
    /*! \brief AbstractMonic テンポラリ受信バッファ */ 
    byte   frame_buf[1500];
    
    /*! \brief 受信パケット本体の長さ */ 
    dword    frame_len;
    
    virtual void frame_input(void) = 0;
    
  public:
  
    //Publicメンバ
    
    /*! \brief 処理待ちフレームリスト */ 
    List<WAIT_FRAME_BUF*>* waitFrameBufList;
    
    /*! \brief MACアドレス */ 
    byte   ether_mac_addr[6];

    
    AbstractMonic();
    //基底クラスなので、仮想デスクトラクタを定義
    virtual ~AbstractMonic();
    
    virtual void enableNetWork(void);
    virtual void disableNetWork(void);
    
    //各種インターフェースメソッド サブクラス(具象クラス(NIC)で実装を期待)
    virtual void frame_output( byte *, byte *, dword, word ) = 0;
    virtual int init(void) = 0;
    virtual int nic_probe(void) = 0;
    //virtual void nic_init(void) = 0;
    virtual int getNicIRQ() = 0;
    virtual void setNicIRQ(int) = 0;
    virtual int getNicIOBASE() = 0;
    virtual void setNicIOBASE(int) = 0;
    
    void frame_input_public(void);
    //void pop_frame_list

};

#endif
