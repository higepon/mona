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

/*!
    abstract class AbstractMonic
*/
class AbstractMonic {

  private:

  protected:


  public:
  
    //Publicメンバ
    /*! \brief AbstractMonic テンポラリ受信バッファ */ 
    byte   frame_buf[1500];
    
    /*! \brief MACアドレス */ 
    byte   ether_mac_addr[6];
    /*! \brief 受信パケット本体の長さ */ 
    dword    frame_len;
    
    AbstractMonic();
    //基底クラスなので、仮想デスクトラクタを定義
    virtual ~AbstractMonic();
    
    virtual void enableNetWork(void);
    virtual void disableNetWork(void);
    
    //各種インターフェースメソッド サブクラス(具象クラス(NIC)で実装を期待)
    virtual void frame_input(void) = 0;
    virtual void frame_output( byte *, byte *, dword, word ) = 0;
    virtual int init(void) = 0;
    virtual int nic_probe(void) = 0;
    //virtual void nic_init(void) = 0;
    virtual int getNicIRQ() = 0;
    virtual void setNicIRQ(int) = 0;
    virtual int getNicIOBASE() = 0;
    virtual void setNicIOBASE(int) = 0;

};

#endif
