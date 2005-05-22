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
    
    static dword packet_get_4byte(byte *buf, int offset);
    static word packet_get_2byte(byte *buf, dword offset);
    static void packet_put_4byte(byte* buf, int offset, dword val);
    static void packet_put_2byte(byte* buf, int offset, word val);
    static word calcCheckSum(dword* ,int );
    static int createPacMsg(MessageInfo *info, char *pac , int pacsize);

    inline static word swapShort(word value)
    {
        return (value>>8)+(value<<8);
    }

    inline static dword swapLong(dword value)
    {
        return (value>>24)+((value>>8)&0xff00)+((value<<8)&0xff0000)+(value<<24);
    }

  private:
    
};

#endif
