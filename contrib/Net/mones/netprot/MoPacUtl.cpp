/*!
    \file   MoPacUtl.cpp
    \brief  Mona パケットユーティリティクラス

    Copyright (c) 2004 Yamami
    All rights reserved.
    License=MIT/X License

    \author  Yamami
    \version $Revision$
    \date   create:2004/08/18 update:$Date$
*/

/*! \class MoPacUtl
 *  \brief Mona パケットユーティリティクラス
 */

#include "MoPacUtl.h"


/*!
    \brief initialize
         MoPacUtl コンストラクタ
    \author Yamami
    \date   create:2004/08/12 update:
*/
MoPacUtl::MoPacUtl()
{

}

/*!
    \brief initialize
         MoEther init
    \author Yamami
    \date   create:2004/08/12 update:
*/



/*!
    \brief initialize
         MoPacUtl デスクトラクタ
    \author Yamami
    \date   create:2004/08/12 update:
*/
MoPacUtl::~MoPacUtl() 
{

}


/*!
    \brief packet_get_4uint8_t
         4バイトパケット取得
         ネットワークエンディアン変換
    \param  uint8_t *buf [in] 対象バッファ
    \param  int offset [in] 対象オフセット
    \return uint32_t 取得値
    
    \author Yamami
    \date   create:2004/08/18 update:
*/
uint32_t MoPacUtl::packet_get_4uint8_t(uint8_t *buf, int offset)
{
    uint32_t a, b, c;

    a = (uint32_t)buf[offset++] << 24;
    b = (uint32_t)buf[offset++] << 16;
    c = (uint32_t)buf[offset++] << 8;

    return a | b | c | buf[offset];
}

/*!
    \brief packet_get_2uint8_t
         2バイトパケット取得
         ネットワークエンディアン変換
    \param  uint8_t *buf [in] 対象バッファ
    \param  int offset [in] 対象オフセット
    \return uint16_t 取得値
    
    \author Yamami
    \date   create:2004/08/18 update:
*/
uint16_t MoPacUtl::packet_get_2uint8_t(uint8_t *buf, uint32_t offset)
{
    uint16_t a;

    a = buf[offset++] << 8;
    
    return a | buf[offset];
}


/*!
    \brief packet_put_4uint8_t
         4バイトパケット設定
         ネットワークエンディアン変換
    \param  uint8_t *buf [in] 対象バッファ
    \param  int offset [in] 対象オフセット
    \param  iuint32_t [in] 設定値
    \return void 無し
    
    \author Yamami
    \date   create:2004/08/18 update:
*/
void MoPacUtl::packet_put_4uint8_t(uint8_t* buf, int offset, uint32_t val)
{
    buf += offset;
    *buf++ = (val >> 24) & 0xff;
    *buf++ = (val >> 16) & 0xff;
    *buf++ = (val >>  8) & 0xff;
    *buf = val & 0xff;
}

/*!
    \brief packet_put_2uint8_t
         2バイトパケット設定
         ネットワークエンディアン変換
    \param  uint8_t *buf [in] 対象バッファ
    \param  int offset [in] 対象オフセット
    \param  iuint16_t [in] 設定値
    \return void 無し
    
    \author Yamami
    \date   create:2004/08/18 update:
*/
void MoPacUtl::packet_put_2uint8_t(uint8_t* buf, int offset, uint16_t val)
{
    buf += offset;
    *buf++ = (val >> 8) & 0xff;
    *buf = val & 0xff;
}



/*!
    \brief calcCheckSum
         チェックサム関数
    \param  uint32_t *data [in] チェック対象
    \param  int size [in] チェック対象サイズ
    \return uint16_t 
    
    \author Yamami
    \date   create:2004/09/20 update:
*/
uint16_t MoPacUtl::calcCheckSum(uint32_t *data,int size)
{
    union{
        unsigned long long u64;
        uint32_t            u32[2];
        uint16_t             u16[4];
    }sum;

    uint32_t tmp;


    sum.u64=0;
    for(;(uint32_t)size>=sizeof(uint32_t);size-=sizeof(uint32_t))
        sum.u64+=*data++;
    if(size>0)sum.u64+=*data&((1<<(size*8))-1);

    tmp=sum.u32[1];
    sum.u32[1]=0;
    sum.u64+=tmp;
    tmp=sum.u32[1];
    sum.u32[1]=0;
    sum.u32[0]+=tmp;

    tmp=sum.u16[1];
    sum.u16[1]=0;
    sum.u32[0]+=tmp;
    tmp=sum.u16[1];
    sum.u16[1]=0;
    sum.u16[0]+=tmp;

    return ~sum.u16[0];
}



/*!
    \brief calcCheckSumDummyHead
         TCP/UDP用。ダミーヘッダ込み チェックサム関数
    \param  uint32_t *dmhead [in] ダミーヘッダ チェックサム対象
    \param  uint32_t *data [in] チェック対象データ
    \param  int dmsize [in] ダミーヘッダ対象サイズ
    \param  int size [in] チェック対象サイズ
    \return uint16_t 
    
    \author Yamami
    \date   create:2005/07/17 update:
*/
uint16_t MoPacUtl::calcCheckSumDummyHead(uint32_t *dmhead,uint32_t *data,int dmsize,int size)
{
    union{
        unsigned long long u64;
        uint32_t               u32[2];
        uint16_t             u16[4];
    }sum;
    
    uint32_t tmp;


    sum.u64=0;
    for(;dmsize>=sizeof(uint32_t);dmsize-=sizeof(uint32_t))
        sum.u64+=*dmhead++;
    for(;size>=sizeof(uint32_t);size-=sizeof(uint32_t))
        sum.u64+=*data++;
    if(size>0)sum.u64+=*data&((1<<(size*8))-1);

    tmp=sum.u32[1];
    sum.u32[1]=0;
    sum.u64+=tmp;
    tmp=sum.u32[1];
    sum.u32[1]=0;
    sum.u32[0]+=tmp;

    tmp=sum.u16[1];
    sum.u16[1]=0;
    sum.u32[0]+=tmp;
    tmp=sum.u16[1];
    sum.u16[1]=0;
    sum.u16[0]+=tmp;

    return ~sum.u16[0];
}


