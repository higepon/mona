/*!
    \file  MoPacUtl.h
    \brief Mona パケットユーティリティクラス ヘッダ

    Copyright (c) 2004 Yamami
    All rights reserved.
    License=MIT/X License

    \author  Yamami 
    \version $Revision$
    \date   create:2004/08/18 update:$Date$
*/
#ifndef _MONA_MOPACUTL_
#define _MONA_MOPACUTL_

#include <sys/types.h>
#include <monapi.h>
#include <monesoc/SocketContainer.h>

//共有メモリ使用
#include <monapi/cmemoryinfo.h>


/*!
    MoEther class
*/
class MoPacUtl
{

    
  public:
    
    //コンストラクタ
    MoPacUtl();
    ~MoPacUtl();
    
    static uint32_t packet_get_4uint8_t(uint8_t *buf, int offset);
    static uint16_t packet_get_2uint8_t(uint8_t *buf, uint32_t offset);
    static void packet_put_4uint8_t(uint8_t* buf, int offset, uint32_t val);
    static void packet_put_2uint8_t(uint8_t* buf, int offset, uint16_t val);
    static uint16_t calcCheckSum(uint32_t* ,int );
    static uint16_t calcCheckSumDummyHead(uint32_t *dmhead,uint32_t *data,int dmsize,int size);
    

    inline static uint16_t swapShort(uint16_t value)
    {
        return (value>>8)+(value<<8);
    }

    inline static uint32_t swapLong(uint32_t value)
    {
        return (value>>24)+((value>>8)&0xff00)+((value<<8)&0xff0000)+(value<<24);
    }

  private:
    
};

#endif
