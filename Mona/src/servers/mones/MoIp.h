/*!
    \file  MoIp.h
    \brief IPプロトコル実装クラス ヘッダ

    Copyright (c) 2004 Yamami
    All rights reserved.
    License=MIT/X Licnese

    \author  Yamami 
    \version $Revision$
    \date   create:2004/09/20 update:$Date$
*/
#ifndef _MONA_MOIP_
#define _MONA_MOIP_

#include <sys/types.h>
#include <sys/HashMap.h>
#include <monapi.h>

#include "MoPacUtl.h"
#include "MonesDefine.h"
#include "AbstractMonic.h"


// Yamami ??この宣言ここでいい？
/*! 
 *  \struct IP_HEADER
 *  \brief IPヘッダ構造体
 */
typedef struct{
    byte  verhead;  /* バージョン、ヘッダ長。 */
    byte  tos;      /* TOS. */
    word len;       /* トータル長。 */
    word id;        /* 識別番号。 */
    word frag;      /* フラグ、フラグメントオフセット。 */
    byte  ttl;      /* Time to Live. */
    byte  prot;     /* プロトコル番号。 */
    word chksum;    /* ヘッダチェックサム。 */
    dword srcip;        /* 送り元IP。 */
    dword dstip;        /* 宛先IP。 */
    char     data[0];
}IP_HEADER;


enum{
    IP_HEAD_VERSION=4<<4,

    IP_HEAD_FRAG_ON= 0x2000,    /* フラグメントデータありフラグ。 */
    IP_HEAD_FRAG_NOT=0x4000,    /* フラグメント不可フラグ。 */
};

/*! 
 *  \struct TRANS_BUF_INFO
 *  \brief IP送信バッファ構造体
 */
typedef struct{
    char  *data[3];     /* 送信フレームアドレス。 */
    int    size[3];     /* データフレームサイズ。 */
    word type;        /* フレームタイプ。 */
    dword  ipType;      /* IPプロトコルタイプ。 */
}TRANS_BUF_INFO;


/*!
    MoIp class
*/
class MoIp
{
    
  public:
    
    //コンストラクタ
    MoIp();
    ~MoIp();
    void initIp(AbstractMonic*);

    int receiveIp(IP_HEADER*);
    int transIp(TRANS_BUF_INFO*, dword , byte , int );

  private:
    int ipRouting(dword ,dword*);
    
    //NICドライバ
    AbstractMonic* insAbstractNic;

};

#endif
