/*!
    \file   SocketContainer.cpp
    \brief  SocketContainerクラス

    Copyright (c) 2005 Yamami
    All rights reserved.
    License=MIT/X License

    \author  Yamami
    \version $Revision$
    \date   create:2005/07/15 update:$Date$
*/

/*! \class SocketContainer
 *  \brief SocketContainerクラス
 */


#include <monesoc/SocketContainer.h>


/*!
    \brief initialize
         SocketContainer コンストラクタ
    \author Yamami
    \date   create:2004/10/11 update:$Date$
*/
SocketContainer::SocketContainer()
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
         SocketContainer デスクトラクタ
    \author Yamami
    \date   create:2004/10/11 update:$Date$
*/
SocketContainer::~SocketContainer()
{
 
}


/*!
    \brief addBuffer
         SocketContainerへ、受信/送信バッファを登録
    \param  char *pac [in] パケット
    \param  int pacsize [in] パケットサイズ
    \param  int rorsFlag [in] 受信?送信?フラグ 0:受信/1:送信
    
    \return int 結果 
    
    \author Yamami
    \date   create:2005/05/22 update:
*/
int SocketContainer::addBuffer(char *pac , int pacsize ,int rorsFlag)
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
    
    
    if(rorsFlag == 0){
        nowRecvBufCount++;
        recvBuff[nowRecvBufCount].Handle = cmInfo->Handle;
        recvBuff[nowRecvBufCount].Owner = cmInfo->Owner;
        recvBuff[nowRecvBufCount].Size = cmInfo->Size;         
        //recvBuff->add(pcm);

    }
    else{
        nowSendBufCount++;
        sendBuff[nowSendBufCount].Handle = cmInfo->Handle;
        sendBuff[nowSendBufCount].Owner = cmInfo->Owner;
        sendBuff[nowSendBufCount].Size = cmInfo->Size;
        //sendBuff->add(pcm);
    }
    
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
packet_cmemoryinfo SocketContainer::getBuffer(int rorsFlag)
{
    packet_cmemoryinfo pcm;
    
    if(rorsFlag == 0){
        pcm = recvBuff[nowRecvBufCount];
        
logprintf("Get nowRecvBufCount=%d\n",nowRecvBufCount);
logprintf("Get pcm.Handle=%d pcm.Owner=%d pcm.Size=%d\n",pcm.Handle , pcm.Owner ,pcm.Size);
        nowRecvBufCount--;        
        
    }
    else{
        pcm = sendBuff[nowSendBufCount];
        
logprintf("Get nowSendBufCount=%d\n",nowSendBufCount);
logprintf("Get pcm.Handle=%d pcm.Owner=%d pcm.Size=%d\n",pcm.Handle , pcm.Owner ,pcm.Size);        
        
        nowSendBufCount--;
        
    }
    
    return pcm;
    
}




/*!
    \brief createPacMonaMsg
         パケット格納メッセージ作成
    \param  MessageInfo *info [OUT] 作成メッセージ
    \param  dword protno [IN] プロトコルNo
    \return int 結果 
    
    \author Yamami
    \date   create:2005/05/22 update:
*/
int SocketContainer::createPacMonaMsg(MessageInfo *info , dword protno )
{

    //共有メモリへSocketContainer格納
    //まず、monapi_cmemoryinfo構造体をnew
    monapi_cmemoryinfo* cmInfo = new monapi_cmemoryinfo();
    
    if (!monapi_cmemoryinfo_create(cmInfo, sizeof(SocketContainer) + 1, 0))
    {
        monapi_cmemoryinfo_delete(cmInfo);
        logprintf("SocketContainer::createPacMonaMsg monapi_cmemoryinfo_create Faild!! \n");
        return 1;
    }
    
    //共有メモリをマップ、Data要素に確保したバイト列がセットされる。
    //Yamami マップいる？？
    monapi_cmemoryinfo_map(cmInfo);
    
    //共有メモリへ、SocketContainer格納 
    memcpy(cmInfo->Data , this, sizeof(SocketContainer));
    
    info->length = sizeof(SocketContainer);
    
    info->arg1 = protno;  //プロトコルNO
    info->arg2 = cmInfo->Handle;
    info->arg3 = cmInfo->Size;
    
    return 0;
  

}
