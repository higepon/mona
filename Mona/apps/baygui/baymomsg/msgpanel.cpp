/*!
    \file  msgpanel.cpp
    \brief 簡易メッセンジャー 結果表示パネルクラス

    Copyright (c) 2005 Yamami
    WITHOUT ANY WARRANTY

    \author  Yamami
    \version $Revision$
    \date   create:2005/05/10 update:$Date$
*/


/*! \class MsgPanel
 *  \brief 簡易メッセージ 結果表示パネルクラス
 */

#include <baygui.h>
#include "msgpanel.h"

//IP関連のヘッダはどこに定義すべきか？
//とりあえず、Mones内のMoDefineを強引にinclude
#include "../../../src/servers/mones/MonesDefine.h"

using namespace MonAPI;

/** コンストラクタ */
MsgPanel::MsgPanel(){
    replayflag = false;
    //保持メッセージリストのインスタンス化
    msglist = new HList<char*>();
    
}

/** IPアドレスを指定する */
void MsgPanel::setIpAddr(char *ipaddr){
    strcpy(this->ipaddr, ipaddr);
    // ここで再描画をしている
    repaint();
}

/** パケットバッファを指定する */
void MsgPanel::setPacbuf(char *pb){
    memcpy(this->pacbuf,pb,this->paclen);
    // ここで再描画をしている
    repaint();
}

/** パケットバッファサイズを指定する */
void MsgPanel::setPaclen(int paclen){
    this->paclen= paclen;
}


/** リプライフラグを指定する。 */
void MsgPanel::setReplayflag(bool rpf){
    replayflag = rpf;
}

/** 再描画（repaintを呼ぶとここにくる）*/
void MsgPanel::paint(Graphics *g){
    // 背景を灰色で塗りつぶす
    g->setColor(getBackground());
    g->fillRect(0, 0, getWidth(), getHeight());
    
    //if (ipaddr != NULL && strlen(ipaddr) > 0) {
        
        if(replayflag == false){


            /*            
            //sprintf(drawBuff , "Pinding %s",ipaddr);
            g->setColor(0,0,0);
            
            //IPアドレス組み立て
            int a,b,c,d;
            sscanf(ipaddr,"%d.%d.%d.%d",&a,&b,&c,&d);
            dword hexip;
            hexip = 0;
            hexip = (byte)a;
            hexip = (hexip << 8) + (byte)b;
            hexip = (hexip << 8) + (byte)c;
            hexip = (hexip << 8) + (byte)d;

            //ここで、Monesにメッセージを送る
            MessageInfo info;

            dword targetID = Message::lookupMainThread("MONES.EX5");
            if (targetID == 0xFFFFFFFF)
            {
                g->drawText("MONES.EX5 not found", 0, 0);
                return;
            }
            
            
            //Monesへ登録
            // create message
            Message::create(&info, MSG_MONES_REGIST, hexip, 2600, 0, NULL);
            // send
            if (Message::send(targetID, &info)) {
                g->drawText("MSG_MONES_REGIST error", 0, 0);
                return;
            }
            
            

            //IP送信
            // create message
            Message::create(&info, MSG_MONES_IP_SEND, 0, 0, 0, NULL);

            //ICMPパケットの作成
            int icmp_size;
            icmp_size=40;
            
            char icmp_buf[40];
            
            ICMP_HEADER *volatile icmpHead;
            icmpHead=(ICMP_HEADER*)icmp_buf;
            
            
            //dataの部分をつくる
            memcpy(icmpHead->data , "abcdefghijklmnopqrstuvwabcdefghi",32);
            
            //ICMPヘッダーの設定
            icmpHead->type=ICMP_TYPE_ECHOREQ;
            icmpHead->code=0;
            //チェックサムはここでは計算しない。Monesに任せる
            //icmpHead->chksum=0;
            //icmpHead->chksum=MoPacUtl::calcCheckSum((dword*)icmpHead,icmp_size);

            info.arg1 = 0;  //共有メモリを使用しない
            info.arg2 = hexip;
            
            memcpy(info.str , icmp_buf,icmp_size);
            info.length = icmp_size;


            // send
            if (Message::send(targetID, &info)) {
                g->drawText("MSG_MONES_IP_SEND error", 0, 0);
                return;
            }
            g->drawText(drawBuff, 0, 0);

            */

        
        }else if(replayflag == true){
            
            g->setColor(0,0,255);
            //IP要求が返ってきたら
            
            if(msglist->size() > MAX_MSG_LINE){
                //先頭行を削除
                char* tmp = msglist->removeAt(0);
                free(tmp);
            }
            
            //メッセージリストへ追加
            char* drawline;
            drawline = (char*)malloc(paclen+1);
            memcpy(drawline, pacbuf , paclen);
            msglist->add(drawline);
            

            //全行表示
            for (int i = 0; i < msglist->size() ; i++) {
                char* tmp = msglist->get(i);
                g->drawText(tmp, 0, i * 16);
            }

        }
        
        
    //}

}
