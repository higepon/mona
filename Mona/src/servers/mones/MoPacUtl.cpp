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
    \brief packet_get_4byte
         4バイトパケット取得
         ネットワークエンディアン変換
    \param  byte *buf [in] 対象バッファ
    \param  int offset [in] 対象オフセット
    \return dword 取得値
    
    \author Yamami
    \date   create:2004/08/18 update:
*/
dword MoPacUtl::packet_get_4byte(byte *buf, int offset)
{
    dword a, b, c;

    a = (dword)buf[offset++] << 24;
    b = (dword)buf[offset++] << 16;
    c = (dword)buf[offset++] << 8;

    return a | b | c | buf[offset];
}

/*!
    \brief packet_get_2byte
         2バイトパケット取得
         ネットワークエンディアン変換
    \param  byte *buf [in] 対象バッファ
    \param  int offset [in] 対象オフセット
    \return word 取得値
    
    \author Yamami
    \date   create:2004/08/18 update:
*/
word MoPacUtl::packet_get_2byte(byte *buf, dword offset)
{
    word a;

    a = buf[offset++] << 8;
    
    return a | buf[offset];
}


/*!
    \brief packet_put_4byte
         4バイトパケット設定
         ネットワークエンディアン変換
    \param  byte *buf [in] 対象バッファ
    \param  int offset [in] 対象オフセット
    \param  idword [in] 設定値
    \return void 無し
    
    \author Yamami
    \date   create:2004/08/18 update:
*/
void MoPacUtl::packet_put_4byte(byte* buf, int offset, dword val)
{
    buf += offset;
    *buf++ = (val >> 24) & 0xff;
    *buf++ = (val >> 16) & 0xff;
    *buf++ = (val >>  8) & 0xff;
    *buf = val & 0xff;
}

/*!
    \brief packet_put_2byte
         2バイトパケット設定
         ネットワークエンディアン変換
    \param  byte *buf [in] 対象バッファ
    \param  int offset [in] 対象オフセット
    \param  iword [in] 設定値
    \return void 無し
    
    \author Yamami
    \date   create:2004/08/18 update:
*/
void MoPacUtl::packet_put_2byte(byte* buf, int offset, word val)
{
    buf += offset;
    *buf++ = (val >> 8) & 0xff;
    *buf = val & 0xff;
}



/*!
    \brief calcCheckSum
         チェックサム関数
    \param  dword *data [in] チェック対象
    \param  int size [in] チェック対象サイズ
    \return word 
    
    \author Yamami
    \date   create:2004/09/20 update:
*/
word MoPacUtl::calcCheckSum(dword *data,int size)
{
    union{
        unsigned long long u64;
        dword            u32[2];
        word             u16[4];
    }sum;

    dword tmp;


    sum.u64=0;
    for(;(dword)size>=sizeof(dword);size-=sizeof(dword))
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
    \param  dword *dmhead [in] ダミーヘッダ チェックサム対象
    \param  dword *data [in] チェック対象データ
    \param  int dmsize [in] ダミーヘッダ対象サイズ
    \param  int size [in] チェック対象サイズ
    \return word 
    
    \author Yamami
    \date   create:2005/07/17 update:
*/
word MoPacUtl::calcCheckSumDummyHead(dword *dmhead,dword *data,int dmsize,int size)
{
    union{
        unsigned long long u64;
        dword               u32[2];
        word             u16[4];
    }sum;
    
    dword tmp;


    sum.u64=0;
    for(;dmsize>=sizeof(dword);dmsize-=sizeof(dword))
        sum.u64+=*dmhead++;
    for(;size>=sizeof(dword);size-=sizeof(dword))
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
    \brief createPacMsg
         パケット格納メッセージ作成
    \param  MessageInfo *info [OUT] 作成メッセージ
    \param  Socket *soc [in] 通知ソケット
    
    \return int 結果 
    
    \author Yamami
    \date   create:2005/05/22 update:
*/
int MoPacUtl::createPacMsg(MessageInfo *info, Socket *soc )
{

    //共有メモリへSocket格納
    //まず、monapi_cmemoryinfo構造体をnew
    monapi_cmemoryinfo* cmInfo = new monapi_cmemoryinfo();
    
    if (!monapi_cmemoryinfo_create(cmInfo, sizeof(Socket) + 1, 0))
    {
        monapi_cmemoryinfo_delete(cmInfo);
        return 1;
    }
    
    //共有メモリをマップ、Data要素に確保したバイト列がセットされる。
    //monapi_cmemoryinfo_map(cmInfo);
    
    //共有メモリへ、Socket格納 
    //TODO 単純にmemcpyでいいのか？
    memcpy(cmInfo->Data , soc, sizeof(Socket));
    
logprintf("sizeof(Socket) = %d \n",sizeof(Socket));

packet_cmemoryinfo pcm;
pcm = soc->recvBuff[0];
logprintf("createPacMsg pcm.Handle=%d pcm.Owner=%d pcm.Size=%d\n",pcm.Handle , pcm.Owner ,pcm.Size);    

pcm = ((Socket*)cmInfo->Data)->recvBuff[0];
logprintf("Cast!!! pcm.Handle=%d pcm.Owner=%d pcm.Size=%d\n",pcm.Handle , pcm.Owner ,pcm.Size);        


    info->length = sizeof(Socket);
    info->arg1 = 1;
    info->arg2 = cmInfo->Handle;
    info->arg3 = cmInfo->Size;
    
    return 0;
  

}
