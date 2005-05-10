/*!
    \file  pingui.cpp
    \brief PingもどきGUIクラス

    Copyright (c) 2005 Yamami
    WITHOUT ANY WARRANTY

    \author  Yamami
    \version $Revision$
    \date   create:2005/05/09 update:$Date$
*/


/*! \class PinGUI
 *  \brief PingもどきGUIクラス
 */

#include <baygui.h>
#include "pingpanel.h"

class PinGUI : public Window {

private:
    TextField *address;
    Button *go;
    PingPanel *panel;

public:
    PinGUI(){
        setRect((800 - 412) / 2, (600 - 428) /2, 412, 428);
        setTitle("pingもどき");
        // いろいろな部品の作成
        go = new Button("ping");
        address = new TextField();
        panel = new PingPanel();
        // いろいろな部品の大きさを設定
        go->setRect(360,20,40,20);
        address->setRect(0,20,360,20);
        panel->setRect(0,40,400,360);
        // いろいろな部品をウィンドウに貼り付ける
        add(address);
        add(go);
        add(panel);
    }
    
    ~PinGUI(){
        free(address);
        free(go);
        free(panel);
    }
    
    void onEvent(Event *event){
        if (event->getType() == MouseEvent::MOUSE_RELEASED) {
            if (event->getSource() == go) {
                // PingPanelクラスに処理をお願いする
                panel->setIpAddr(address->getText());
            }
        } else if (event->getType() == Event::TEXT_CHANGED) {
            // 確定したとき
            if (event->getSource() == address) {
                // PingPanelクラスに処理をお願いする
                //panel->setIpAddr(address->getText());
            }
        } else if (event->getType() == Event::CUSTOM_EVENT && event->header == MSG_MONES_ICMP_NOTICE) {
            // Monesから、
            panel->setReplayflag(true);
            panel->setPaclen(event->length);
            panel->setPacbuf(event->str);
        }
        
        
    }
};

int MonaMain(List<char*>* pekoe) {
    PinGUI *ppingui = new PinGUI();
    ppingui->run();
    delete(ppingui);
    return 0;
}
