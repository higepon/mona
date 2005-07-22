/*!
    \file   Socket.cpp
    \brief  簡易Socketクラス

    Copyright (c) 2005 Yamami
    All rights reserved.
    License=MIT/X License

    \author  Yamami
    \version $Revision$
    \date   create:2005/07/15 update:$Date$
*/

/*! \class Socket
 *  \brief 簡易Socketクラス
 */


#include <monesoc/Socket.h>


/*!
    \brief initialize
         Socket コンストラクタ
    \author Yamami
    \date   create:2004/10/11 update:$Date$
*/
Socket::Socket()
{
    //受信バッファインスタンス化(共有メモリ)
    //recvBuff = new HList<packet_cmemoryinfo*>();
    
    //送信バッファインスタンス化(共有メモリ)
    //sendBuff = new HList<packet_cmemoryinfo*>(); 
    
    nowRecvBufCount = -1;
    nowSendBufCount = -1;
}

/*!
    \brief initialize
         Socket デスクトラクタ
    \author Yamami
    \date   create:2004/10/11 update:$Date$
*/
Socket::~Socket()
{
 
}


/*!
    \brief addBuffer
         Socketへ、受信/送信バッファを登録
    \param  char *pac [in] パケット
    \param  int pacsize [in] パケットサイズ
    \param  int rorsFlag [in] 受信?送信?フラグ 0:受信/1:送信
    
    \return int 結果 
    
    \author Yamami
    \date   create:2005/05/22 update:
*/
int Socket::addBuffer(char *pac , int pacsize ,int rorsFlag)
{

    //共有メモリへパケット格納
    //まず、monapi_cmemoryinfo構造体をnew
    monapi_cmemoryinfo* cmInfo = new monapi_cmemoryinfo();
    
    if (!monapi_cmemoryinfo_create(cmInfo, (dword)pacsize + 1, 0))
    {
        monapi_cmemoryinfo_delete(cmInfo);
        return 1;
    }
    
    
    //共有メモリをマップ、Data要素に確保したバイト列がセットされる。
    //create側でMAPはいらない？？
    //monapi_cmemoryinfo_map(cmInfo);
    
    //共有メモリへ、パケットセット
    memcpy(cmInfo->Data , pac, pacsize);
    
    
    //※※ 受信、送信バッファに登録するのはあくまでハンドル等のみ
    

logprintf("Pre!!! if(rorsFlag == 0)\n");  
logprintf("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n");  

    
    if(rorsFlag == 0){
logprintf("flag0 ON\n");
        nowRecvBufCount++;
logprintf("nowRecvBufCount=%d\n",nowRecvBufCount);

        recvBuff[nowRecvBufCount].Handle = cmInfo->Handle;
        recvBuff[nowRecvBufCount].Owner = cmInfo->Owner;
        recvBuff[nowRecvBufCount].Size = cmInfo->Size;         
        //recvBuff->add(pcm);

logprintf("Add pcm.Handle=%d pcm.Owner=%d pcm.Size=%d\n",recvBuff[nowRecvBufCount].Handle , recvBuff[nowRecvBufCount].Owner ,recvBuff[nowRecvBufCount].Size);

    }
    else{
logprintf("flag1 ON\n");        
        nowSendBufCount++;
        sendBuff[nowSendBufCount].Handle = cmInfo->Handle;
        sendBuff[nowSendBufCount].Owner = cmInfo->Owner;
        sendBuff[nowSendBufCount].Size = cmInfo->Size;
        
        //sendBuff->add(pcm);
    }
    

logprintf("After!!! pcm.Handle Set;\n");
    
    return 0;
}


/*!
    \brief getBuffer
         カレントのバッファを取得する。
    \param  int rorsFlag [in] 受信?送信?フラグ 0:受信/1:送信
    
    \return int 結果 
    
    \author Yamami
    \date   create:2005/05/22 update:
*/
packet_cmemoryinfo Socket::getBuffer(int rorsFlag)
{
    packet_cmemoryinfo pcm;
    
    if(rorsFlag == 0){
        pcm = recvBuff[nowRecvBufCount];
        
logprintf("Get nowRecvBufCount=%d\n",nowRecvBufCount);
logprintf("Get pcm.Handle=%d pcm.Owner=%d pcm.Size=%d\n",pcm.Handle , pcm.Owner ,pcm.Size);
        nowRecvBufCount--;        
        
    }
    else{
        pcm = recvBuff[nowRecvBufCount];
        nowSendBufCount--;
        
    }
    
    return pcm;
    
}

