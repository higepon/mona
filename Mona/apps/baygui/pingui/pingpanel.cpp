/*!
    \file  pingpanel.cpp
    \brief Pingもどき 結果表示パネルクラス

    Copyright (c) 2005 Yamami
    WITHOUT ANY WARRANTY

    \author  Yamami
    \version $Revision$
    \date   create:2005/05/09 update:$Date$
*/


/*! \class PingPanel
 *  \brief Pingもどき 結果表示パネルクラス
 */

#include <baygui.h>
#include "pingpanel.h"

//IP関連のヘッダはどこに定義すべきか？
//とりあえず、Mones内のMoDefineを強引にinclude
#include "../../../src/servers/mones/MonesDefine.h"

using namespace MonAPI;

/** コンストラクタ */
PingPanel::PingPanel(){
    replayflag = false;
}

/** IPアドレスを指定する */
void PingPanel::setIpAddr(char *ipaddr){
    strcpy(this->ipaddr, ipaddr);
    // ここで再描画をしている
    repaint();
}

/** パケットバッファを指定する */
void PingPanel::setPacbuf(char *pb){
    memcpy(this->pacbuf,pb,this->paclen);
    // ここで再描画をしている
    repaint();
}

/** パケットバッファサイズを指定する */
void PingPanel::setPaclen(int paclen){
    this->paclen= paclen;
}


/** リプライフラグを指定する。 */
void PingPanel::setReplayflag(bool rpf){
    replayflag = rpf;
}

/** 再描画（repaintを呼ぶとここにくる）*/
void PingPanel::paint(Graphics *g){
    // 背景を灰色で塗りつぶす
    g->setColor(getBackground());
    g->fillRect(0, 0, getWidth(), getHeight());
    
    if (ipaddr != NULL && strlen(ipaddr) > 0) {
        char drawBuff[128];
        
        if(replayflag == false){
            
            sprintf(drawBuff , "Pinding %s",ipaddr);
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
            Message::create(&info, MSG_MONES_REGIST, hexip, 0, 0, NULL);
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

        
        }else if(replayflag == true){
            
            g->setColor(0,0,255);
            //IP要求が返ってきたら
            sprintf(drawBuff , "Reply from %s ",ipaddr);
            g->drawText(drawBuff, 0, 16);
            sprintf(drawBuff , "Size %d ",paclen);
            g->drawText(drawBuff, 0, 32);
            
            for(int i = 0 ; i < paclen ; i++){
                sprintf(drawBuff , "%2x",(byte)pacbuf[i]);
                g->drawText(drawBuff, (i%10)*30, ((i/10)+3)*16);
                //g->drawText(drawBuff, 0, (i+3)*16);
            }
            
            replayflag = false;
        }
        
        
    }

}
