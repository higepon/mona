/*!
    \file  baymomsg.cpp
    \brief 簡易メッセージ Mona側

    Copyright (c) 2005 Yamami
    WITHOUT ANY WARRANTY

    \author  Yamami
    \version $Revision$
    \date   create:2005/05/10 update:$Date$
*/


/*! \class BayMoMsg
 *  \brief 簡易メッセージGUIクラス
 */

#include <baygui.h>
#include "msgpanel.h"

using namespace MonAPI;

class BayMoMsg : public Window {

private:
    TextField *address;
    TextField *port;
    TextField *msg;
    Button *go;
    MsgPanel *panel;

public:
    BayMoMsg(){
        setRect((800 - 412) / 2, (600 - 428) /2, 412, 428);
        setTitle("BayMoMsg");
        // いろいろな部品の作成
        address = new TextField();
        port = new TextField();
        msg = new TextField();
        go = new Button("送信");
        panel = new MsgPanel();
        // いろいろな部品の大きさを設定
        address->setRect(0,20,300,20);
        port->setRect(310,20,50,20);
        msg->setRect(0,40,360,20);
        go->setRect(360,40,40,20);
        
        panel->setRect(0,60,400,340);
        
        
        //初期値設定
        address->setText("192.168.0.1");
        //port->setText("");
        //msg->setText("TO Win Message From Mona!!");
        
        // いろいろな部品をウィンドウに貼り付ける
        add(address);
        add(port);
        add(msg);
        add(go);
        add(panel);
        
        
        //ここでMonesへポート番号登録
        MessageInfo info;

        dword targetID = Message::lookupMainThread("MONES.EX5");
        if (targetID == 0xFFFFFFFF)
        {
            return;
        }

        Message::create(&info, MSG_MONES_REGIST, 0, 2600, 0, NULL);
        // send
        if (Message::send(targetID, &info)) {
            return;
        }

    }
    
    ~BayMoMsg(){
        free(address);
        free(port);     
        free(msg);
        free(go);
        free(panel);
    }
    
    void onEvent(Event *event){
        if (event->getType() == MouseEvent::MOUSE_RELEASED) {
            if (event->getSource() == go) {
                // MsgPanelクラスに処理をお願いする
                panel->setIpAddr(address->getText());
            }
        } else if (event->getType() == Event::TEXT_CHANGED) {
            // 確定したとき
            if (event->getSource() == address) {
                // MsgPanelクラスに処理をお願いする
                //panel->setIpAddr(address->getText());
            }
        } else if (event->getType() == Event::CUSTOM_EVENT && event->header == MSG_MONES_ICMP_NOTICE) {
            // Monesから、
            panel->setReplayflag(true);
            panel->setPaclen(event->length);
            
            logprintf("event->arg1 = %d \n",event->arg1);
            
            if(event->arg1 == 1){
                //共有メモリ
                logprintf("use commonMem Handle = %d : Size = %d \n",event->arg2 , event->arg3);
                monapi_cmemoryinfo* cmPac;

                cmPac = monapi_cmemoryinfo_new();
                cmPac->Handle = event->arg2;
                //cmPac->Owner  = tid;
                cmPac->Size   = event->arg3;
                monapi_cmemoryinfo_map(cmPac);
                panel->setPacbuf((char*)cmPac->Data);
                
                monapi_cmemoryinfo_dispose(cmPac);
                monapi_cmemoryinfo_delete(cmPac);
                
            }else{
                panel->setPacbuf(event->str);
            }
            
        }
        
        
    }
};

int MonaMain(List<char*>* pekoe) {
    BayMoMsg *pbaymomsg = new BayMoMsg();
    pbaymomsg->run();
    delete(pbaymomsg);
    return 0;
}
