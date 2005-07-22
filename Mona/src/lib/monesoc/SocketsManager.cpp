/*!
    \file   SocketsManager.cpp
    \brief  Socket群 管理クラス

    Copyright (c) 2005 Yamami
    All rights reserved.
    License=MIT/X License

    \author  Yamami
    \version $Revision$
    \date   create:2005/07/15 update:$Date$
*/

/*! \class SocketsManager
 *  \brief Socket群 管理クラス
 */


#include <monesoc/SocketsManager.h>

/*!
    \brief initialize
         SocketsManager コンストラクタ
    \author Yamami
    \date   create:2004/10/11 update:$Date$
*/
SocketsManager::SocketsManager()
{
    //コネクション、およびリスンハッシュをインスタンス化
    conSocHash = new HashMap<Socket*>(MAX_SOC_SIZE);
    lisSocHash = new HashMap<Socket*>(MAX_SOC_SIZE);    
}


/*!
    \brief findSocket
         ソケット検索処理
    \param  dword ip [in] 先IPアドレス
    \param  word  myport [in] 自己ポート番号
    \param  word  youport[in] 相手ポート番号
    \return Socket*  検索したSocketへのポインタ。見つからない場合はNULLポインタ
        
    \author Yamami
    \date   create:2005/07/20 update:
*/
Socket* SocketsManager::findSocket(dword ip ,word  myport ,word youport)
{
    char SocKey[30];    //Socketキー
    Socket *retSoc;     //Socketリターン
    
    //コネクションソケットから探す
    //MAPのキー化(String化)
    sprintf(SocKey , "%08x%08x%08x",myport,ip,youport);
    retSoc = conSocHash->get(SocKey);
    
    if(retSoc!=NULL){
        return retSoc;
    }
    
    //リスンソケットから探す
    //MAPのキー化(String化)
    sprintf(SocKey , "%08x",myport);
    retSoc = lisSocHash->get(SocKey);
    
    if(retSoc!=NULL){
        return retSoc;
    }    

    return NULL;
}


/*!
    \brief registLisSocket
         リスンソケット登録処理
    \param  dword tid [in] スレッドid
    \param  word  myport [in] 自己ポート番号
    \return 無し
        
    \author Yamami
    \date   create:2005/07/20 update:
*/
void SocketsManager::registLisSocket(dword tid ,word  myport )
{
    Socket *soc;
    
    soc = new Socket();
    soc->tid = tid;
    soc->myport = myport;
    soc->status = LISTEN;

    char SocKey[30];    //Socketキー
    
    sprintf(SocKey , "%08x",myport);
    lisSocHash->put(SocKey , soc);
}


/*!
    \brief registConSocket
         コネクションソケット登録処理
    \param  dword tid [in] スレッドid
    \param  word  myport [in] 自己ポート番号
    \param  dword ip [in] 先IPアドレス
    \param  word  youport[in] 相手ポート番号    
    \return 無し
        
    \author Yamami
    \date   create:2005/07/20 update:
*/
void SocketsManager::registConSocket(dword tid ,word myport, dword ip , word youport)
{
    Socket *soc;
    
    soc = new Socket();
    soc->tid = tid;
    soc->myport = myport;
    soc->ip = ip;
    soc->youport = youport; 
    soc->status = ESTABLISHED;

    char SocKey[30];    //Socketキー
    
    sprintf(SocKey , "%08x%08x%08x",myport,ip,youport);
    conSocHash->put(SocKey , soc);
}
